  LogicalResult matchAndRewrite(AffineOpType fc_op,
                                PatternRewriter &rewriter) const override {
    // Binary op.
    Operation *binary_op = fc_op.input().getDefiningOp();
    if (!binary_op || binary_op->getNumOperands() != 2) return failure();
    // We only handle the cases the RHS is a scalar.
    // TODO(fengliuai): Currently the canonicalizer pass couldn't guarantee that
    // the constant operands are on the RHS, we need to consider LHS constant
    // operand if necessary.
    DenseFPElementsAttr cst;
    if (!matchPattern(binary_op->getOperand(1), m_Constant(&cst)))
      return failure();
    if (cst.getNumElements() != 1) return failure();
    APFloat cst_value = *cst.float_value_begin();

    // Affine op.
    Value filter = fc_op.filter();
    Value bias = fc_op.bias();
    DenseFPElementsAttr filter_cst, bias_cst;
    if (!matchPattern(filter, m_Constant(&filter_cst))) {
      // The filter maybe quantized, then we should set it to the real constant.
      auto dq = llvm::dyn_cast_or_null<DequantizeOp>(filter.getDefiningOp());
      if (!dq) return failure();
      auto q = llvm::dyn_cast_or_null<QuantizeOp>(dq.input().getDefiningOp());
      if (!q || !matchPattern(q.input(), m_Constant(&filter_cst))) {
        return failure();
      }
      filter = q.input();
    }
    if (!bias.getType().isa<NoneType>() &&
        !matchPattern(bias, m_Constant(&bias_cst)))
      return failure();
    auto binary_op_activation_func =
        binary_op->template getAttrOfType<StringAttr>(
            "fused_activation_function");
    if (!binary_op_activation_func ||
        binary_op_activation_func.getValue() != "NONE")
      return failure();
    ShapedType filter_type = filter_cst.getType();

    if (llvm::isa<AddOp, SubOp>(binary_op)) {
      auto padding = fc_op->template getAttrOfType<StringAttr>("padding");
      if (padding && padding.getValue() != "VALID") return failure();

      // The fusion of add/sub is actually applying the following
      // transformation:
      // w * (x + c) + b => w * x + (w * c + b)
      // so we have to update the bias.
      if (llvm::isa<SubOp>(binary_op)) cst_value.changeSign();

      auto bias_and_slice =
          GetBiasDimAndSliceSize(filter_type.getShape(), fc_op);
      int64_t bias_size = bias_and_slice.first;
      int64_t slice_size = bias_and_slice.second;
      ShapedType new_bias_type =
          RankedTensorType::get({bias_size}, filter_type.getElementType());

      // The new bias should be a 1-D tensor with length equals to the bias
      // dimension of the weight.
      SmallVector<APFloat, 4> new_bias_values;
      if (bias.getType().isa<NoneType>()) {  // none bias, a list of zeros
        new_bias_values.resize(bias_size,
                               APFloat::getZero(cst_value.getSemantics()));
      } else if (bias_cst.getNumElements() == 1) {  // scalar bias, broadcast it
        new_bias_values.resize(bias_size, *bias_cst.float_value_begin());
      } else if (bias_cst.getNumElements() == bias_size) {  // 1-d bias, copy it
        new_bias_values.insert(new_bias_values.begin(),
                               bias_cst.float_value_begin(),
                               bias_cst.float_value_end());
      } else {
        return failure();
      }

      int64_t flatten_index = 0;
      for (auto fp_it = filter_cst.float_value_begin(),
                fp_end = filter_cst.float_value_end();
           fp_it != fp_end; ++fp_it) {
        int bias_index = (flatten_index++ / slice_size) % bias_size;

        new_bias_values[bias_index] =
            new_bias_values[bias_index] + *fp_it * cst_value;
      }
      auto new_bias = DenseFPElementsAttr::get(new_bias_type, new_bias_values);
      auto new_bias_op =
          rewriter.create<ConstOp>(fc_op.getLoc(), new_bias_type, new_bias);
      fc_op.setOperand(0, binary_op->getOperand(0));
      fc_op.setOperand(2, new_bias_op);
    } else if (llvm::isa<MulOp, DivOp>(binary_op)) {
      // The fusion of mul/div is actually applying the following
      // transformation:
      // w * (x ' c) + b => (w ' c) x + b
      // so we have to update the weight.
      bool is_mul = llvm::isa<MulOp>(binary_op);
      auto new_filter =
          filter_cst.mapValues(filter_type.getElementType(), [&](APFloat it) {
            return (is_mul ? it * cst_value : it / cst_value).bitcastToAPInt();
          });
      // We recreate the constant op in case it is shared by the other ops. This
      // might increase the model size.
      auto new_filter_op = rewriter.create<ConstOp>(
          fc_op.getLoc(), filter.getType(), new_filter);
      fc_op.setOperand(0, binary_op->getOperand(0));
      if (fc_op.filter() != filter) {
        // This filter goes through quantize and dequantize ops. Then we just
        // need to update the weight to the quantize op.
        filter.replaceAllUsesWith(new_filter_op);
      } else {
        // This filter doesn't go through quantize and dequantize ops, Then
        // we update the weight of the affine op directly.
        fc_op.setOperand(1, new_filter_op);
      }
    } else {
      return failure();
    }
    return success();
  }