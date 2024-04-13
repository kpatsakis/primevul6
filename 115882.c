  LogicalResult matchAndRewrite(TFL::FullyConnectedOp fully_connected_op,
                                PatternRewriter &) const override {
    auto input = fully_connected_op.input();
    auto input_ty = input.getType().dyn_cast<ShapedType>();
    auto output_ty = fully_connected_op.output()[0]
                         .getType()
                         .template dyn_cast<ShapedType>();
    if (!input_ty.hasStaticShape() ||
        fully_connected_op.weights_format() != "DEFAULT" ||
        fully_connected_op.keep_num_dims() || !output_ty.hasStaticShape() ||
        output_ty.getRank() != 2) {
      return failure();
    }

    auto reshape_op = input.getDefiningOp<TFL::ReshapeOp>();
    if (!reshape_op) return failure();

    // Check if the last dimension does not change after reshape.
    auto reshape_input = reshape_op.input();
    auto reshape_input_ty = reshape_input.getType().dyn_cast<ShapedType>();
    if (!reshape_input_ty.hasStaticShape() || input_ty.getRank() == 0 ||
        reshape_input_ty.getRank() == 0 ||
        input_ty.getDimSize(input_ty.getRank() - 1) !=
            reshape_input_ty.getDimSize(reshape_input_ty.getRank() - 1)) {
      return failure();
    }

    // Connect the input to the one of reshape.
    fully_connected_op.setOperand(0, reshape_input);
    return success();
  }