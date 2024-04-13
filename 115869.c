  LogicalResult matchAndRewrite(TFL::ReshapeOp reshape_op,
                                PatternRewriter &rewriter) const override {
    auto fully_connected_op = llvm::dyn_cast_or_null<TFL::FullyConnectedOp>(
        reshape_op.input().getDefiningOp());
    if (!fully_connected_op || fully_connected_op.getNumResults() != 1 ||
        fully_connected_op.weights_format() != "DEFAULT" ||
        fully_connected_op.keep_num_dims())
      return failure();
    if (!reshape_op.input().hasOneUse()) return failure();

    auto input_shape = fully_connected_op.input().getType().cast<ShapedType>();
    auto output_shape = fully_connected_op.getType(0).cast<ShapedType>();
    auto reshape_shape = reshape_op.getType().cast<ShapedType>();
    if (!input_shape.hasStaticShape() || !output_shape.hasStaticShape() ||
        !reshape_shape.hasStaticShape())
      return failure();

    // Check that the reshape doesn't modify the last dimension and it restores
    // the input (batch) dimension with the exception of the feature (last)
    // dimension.
    if (output_shape.getShape().empty() || reshape_shape.getShape().empty() ||
        output_shape.getShape().back() != reshape_shape.getShape().back() ||
        input_shape.getShape().drop_back() !=
            reshape_shape.getShape().drop_back())
      return failure();

    llvm::SmallVector<Type, 1> output_type{reshape_op.getType()};
    rewriter.replaceOpWithNewOp<TFL::FullyConnectedOp>(
        reshape_op, output_type, fully_connected_op.input(),
        fully_connected_op.filter(), fully_connected_op.bias(),
        fully_connected_op.fused_activation_function(),
        fully_connected_op.weights_format(), /*keep_num_dims=*/true);
    return success();
  }