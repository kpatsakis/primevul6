  LogicalResult matchAndRewrite(TFL::MulOp mul_op,
                                PatternRewriter &rewriter) const override {
    // Mul. Required 1-D rhs for batch normalization.
    DenseElementsAttr gamma_cst;
    Value gamma = mul_op.rhs();
    if (!matchPattern(gamma, m_Constant(&gamma_cst))) return failure();
    if (gamma_cst.getType().getRank() != 1) return failure();

    // Affine op
    Operation *mul_op_lhs = mul_op.lhs().getDefiningOp();
    auto fc_op = dyn_cast_or_null<AffineOpType>(mul_op_lhs);
    if (!fc_op) return failure();
    Value filter = fc_op.filter();
    Value bias = fc_op.bias();

    // QDQs
    auto dq_op = dyn_cast_or_null<TFL::DequantizeOp>(filter.getDefiningOp());
    if (!dq_op) return failure();
    auto q_op =
        dyn_cast_or_null<TFL::QuantizeOp>(dq_op.input().getDefiningOp());
    if (!q_op) return failure();
    filter = q_op.input();

    // weight constant
    ElementsAttr cst_tmp;
    if (!matchPattern(filter, m_Constant(&cst_tmp))) return failure();
    if (!bias.getType().isa<NoneType>() &&
        !matchPattern(bias, m_Constant(&cst_tmp)))
      return failure();
    if (fc_op.fused_activation_function() != "NONE") return failure();

    // Broadcast the constant operand of Mul if it isn't compatible to the
    // filter input. We only support broadcasting the operand along the depth
    // dimension, when the operand's depth is 1.
    rewriter.setInsertionPoint(q_op);
    Location loc = fc_op.getLoc();
    Value broadcasted_gamma;
    if (isa<TFL::Conv2DOp>(mul_op_lhs)) {
      auto mul_rhs = ExpandTo4DForConv(gamma_cst);
      broadcasted_gamma = rewriter.create<ConstOp>(loc, mul_rhs);
    } else if (isa<TFL::DepthwiseConv2DOp>(mul_op_lhs)) {
      auto mul_rhs = ExpandTo4DForDepthwiseConv(gamma_cst);
      broadcasted_gamma = rewriter.create<ConstOp>(loc, mul_rhs);
    } else {
      return failure();
    }

    // Rewrite filter constant. Since the folder of TFL::MulOp couldn't
    // broadcast the operands, TF::MulOp is used to fold the constant.
    auto new_filter =
        rewriter.create<TF::MulOp>(loc, filter, broadcasted_gamma).z();
    // Update the scale in the quantize op.
    auto new_qtype = RescaleQtype(q_op.qtype(), gamma_cst);
    if (!new_qtype) return failure();
    rewriter.replaceOpWithNewOp<TFL::QuantizeOp>(q_op, new_qtype.getValue(),
                                                 new_filter, new_qtype);

    // If bias isn't None, it needs to be multiplied as well.
    if (!bias.getType().isa<NoneType>()) {
      rewriter.setInsertionPoint(fc_op);
      auto new_bias = rewriter.create<TF::MulOp>(loc, bias, gamma);
      fc_op.getOperation()->replaceUsesOfWith(bias, new_bias);
    }

    // Remove the tailing mul op.
    mul_op.replaceAllUsesWith(fc_op.getResult());
    return success();
  }