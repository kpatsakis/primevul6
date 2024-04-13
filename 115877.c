  LogicalResult matchAndRewrite(TFL::FullyConnectedOp fc_op,
                                PatternRewriter &rewriter) const override {
    // This only works with default format.
    if (fc_op.weights_format() != "DEFAULT") return failure();

    // Match Mul.
    auto mul_op = dyn_cast_or_null<TFL::MulOp>(fc_op.input().getDefiningOp());
    if (!mul_op) return failure();
    if (mul_op.fused_activation_function() != "NONE") return failure();

    // Don't match muls where the multiplier constant is not 1D.
    {
      auto multiplier_shape = mul_op.rhs().getType().cast<ShapedType>();
      if (!multiplier_shape.hasStaticShape()) return failure();
      if (multiplier_shape.getShape().size() != 1) return failure();
    }

    // We rely on constant folding, implemented only for F32. Check types.
    if (!IsF32Value(mul_op.rhs()) || !IsF32Value(fc_op.filter())) {
      return failure();
    }

    auto location =
        FusedLoc::get(mul_op.getContext(), {mul_op.getLoc(), fc_op.getLoc()});

    auto new_filter = rewriter.create<TFL::MulOp>(
        location,
        /*lhs=*/fc_op.filter(),
        /*rhs=*/mul_op.rhs(),
        /*fused_activation_function=*/rewriter.getStringAttr("NONE"));
    // Create the updated FC.
    auto new_fc = rewriter.create<TFL::FullyConnectedOp>(
        location, fc_op.output().getTypes(),
        /*input=*/mul_op.lhs(),
        /*filter=*/new_filter,
        /*bias=*/fc_op.bias(),
        /*fused_activation_function=*/
        rewriter.getStringAttr(fc_op.fused_activation_function()),
        /*weights_format=*/rewriter.getStringAttr("DEFAULT"),
        /*keep_num_dims=*/rewriter.getBoolAttr(fc_op.keep_num_dims()));
    rewriter.replaceOp(fc_op.getOperation(), new_fc.output());

    return success();
  }