  LogicalResult matchAndRewrite(ReluXOp relu_op,
                                PatternRewriter &rewriter) const override {
    Operation *input = relu_op.getOperand().getDefiningOp();
    if (!isa_and_nonnull<FullyConnectedOp>(input)) return failure();
    auto fully_connected_op = cast<FullyConnectedOp>(input);
    if (fully_connected_op.fused_activation_function() != "NONE")
      return failure();

    auto new_activation_func = rewriter.getStringAttr(Act);
    auto new_weights_format =
        rewriter.getStringAttr(fully_connected_op.weights_format());
    auto new_keep_num_dims =
        rewriter.getBoolAttr(fully_connected_op.keep_num_dims());
    auto fc = rewriter.create<FullyConnectedOp>(
        FusedLoc::get(relu_op.getContext(),
                      {fully_connected_op.getLoc(), relu_op.getLoc()}),
        relu_op.getType(), fully_connected_op.input(),
        fully_connected_op.filter(), fully_connected_op.bias(),
        new_activation_func, new_weights_format, new_keep_num_dims);
    rewriter.replaceOp(relu_op, fc.output());

    return success();
  }