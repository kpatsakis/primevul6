  LogicalResult matchAndRewrite(TFL::FullyConnectedOp fc_op,
                                PatternRewriter &rewriter) const override {
    // This only works with default format.
    if (fc_op.weights_format() != "DEFAULT") return failure();

    // Match Add.
    auto add_op = dyn_cast_or_null<TFL::AddOp>(fc_op.input().getDefiningOp());
    if (!add_op) return failure();
    if (add_op.fused_activation_function() != "NONE") return failure();

    // Don't match adds where the added constant is not 1D.
    {
      auto addend_shape = add_op.rhs().getType().cast<ShapedType>();
      if (!addend_shape.hasStaticShape()) return failure();
      if (addend_shape.getShape().size() != 1) return failure();
    }

    // Calculate new bias.  Generate a new FC; it will be constant folded.
    auto old_bias = fc_op.bias();
    if (!old_bias || old_bias.getType().isa<NoneType>()) {
      // TODO(b/180752069): Figure out new bias' type when old bias is empty.
      return failure();
    }

    // The FC relies on constant folding, which is implemented on F32. Checks
    // types to be F32.
    {
      if (!IsF32Value(add_op.rhs()) || !IsF32Value(fc_op.filter()) ||
          !IsF32Value(old_bias))
        return failure();
    }

    auto new_bias = rewriter.create<TFL::FullyConnectedOp>(
        fc_op.getLoc(), old_bias.getType(),
        /*input=*/add_op.rhs(),
        /*filter=*/fc_op.filter(),
        /*bias=*/old_bias,
        /*fused_activation_function=*/rewriter.getStringAttr("NONE"),
        /*weights_format=*/rewriter.getStringAttr("DEFAULT"),
        /*keep_num_dims=*/rewriter.getBoolAttr(true));

    // Create the updated FC.
    auto new_fc = rewriter.create<TFL::FullyConnectedOp>(
        FusedLoc::get(add_op.getContext(), {add_op.getLoc(), fc_op.getLoc()}),
        fc_op.output().getTypes(),
        /*input=*/add_op.lhs(),
        /*filter=*/fc_op.filter(),
        /*bias=*/*new_bias.output().begin(),
        /*fused_activation_function=*/
        rewriter.getStringAttr(fc_op.fused_activation_function()),
        /*weights_format=*/rewriter.getStringAttr("DEFAULT"),
        /*keep_num_dims=*/rewriter.getBoolAttr(fc_op.keep_num_dims()));
    rewriter.replaceOp(fc_op.getOperation(), new_fc.output());

    return success();
  }