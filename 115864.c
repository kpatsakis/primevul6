  LogicalResult matchAndRewrite(BinaryOpType binary_op,
                                PatternRewriter &rewriter) const override {
    DenseElementsAttr splat_elements_attr;
    if (!IsScalarizableSplatConstant(binary_op.rhs(), &splat_elements_attr)) {
      return failure();
    }

    constexpr int kSplatOperandIndex = 1;
    auto result_type =
        binary_op.getResult().getType().template cast<ShapedType>();
    mlir::Value non_splat_operand =
        binary_op.getOperand(1 - kSplatOperandIndex);
    auto non_splat_operand_type =
        non_splat_operand.getType().cast<ShapedType>();
    // If the other operand's shape does not equal to the result shape, then we
    // cannot scalarize the splat constant because the result shape relies on
    // the splat constant op's shape for broadcasting.
    if (!non_splat_operand_type.hasStaticShape() ||
        non_splat_operand_type.getShape() != result_type.getShape() ||
        non_splat_operand_type.getRank() > 4) {
      return failure();
    }

    // If non-splat operand is not fusable affine ops, then no need to apply
    // this transformation.
    if (!CanFuseAffineOp(non_splat_operand.getDefiningOp(), binary_op)) {
      return failure();
    }

    // Creates a new scalar constant op using the splat value.
    mlir::Value splat_operand = binary_op.getOperand(kSplatOperandIndex);
    auto scalar_elements_attr = DenseElementsAttr::get(
        RankedTensorType::get({},
                              splat_elements_attr.getType().getElementType()),
        splat_elements_attr.getSplatValue());

    auto scalar_constant_op = rewriter.create<ConstantOp>(
        splat_operand.getLoc(), scalar_elements_attr.getType(),
        scalar_elements_attr);

    binary_op.setOperand(kSplatOperandIndex, scalar_constant_op);
    return success();
  }