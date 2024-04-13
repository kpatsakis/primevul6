  LogicalResult matchAndRewrite(TFL::AddOp add_op,
                                PatternRewriter &rewriter) const override {
    // Match Add.
    DenseElementsAttr added_value;
    Value constant_val = add_op.rhs();
    if (!matchPattern(constant_val, m_Constant(&added_value))) return failure();

    // Match Fully Connected.
    auto fc_op =
        dyn_cast_or_null<TFL::FullyConnectedOp>(add_op.lhs().getDefiningOp());
    if (!fc_op) return failure();

    // Check if the constant RHS is either 0D (scalar), or a 1D with
    // `{num_channels}` shape.
    auto constant_val_type = constant_val.getType().cast<TensorType>();

    // In TFLite FullyConnect definition, bias must be a 1D tensor where
    // the number of elements is equal to the number of channels.
    // If it's not 1D or 0D (which can be broadcasted to 1D), reject the
    // matching.
    bool is_scalar_rhs = false;
    if (constant_val_type.getRank() == 0) {
      is_scalar_rhs = true;
    } else if (constant_val_type.getRank() != 1) {
      return failure();
    }

    Value filter = fc_op.filter();
    Value bias = fc_op.bias();
    ElementsAttr bias_value;
    const bool is_none_bias = bias.getType().isa<NoneType>();
    if (fc_op.fused_activation_function() != "NONE") return failure();

    if (!is_none_bias && !matchPattern(bias, m_Constant(&bias_value)))
      return failure();

    // Rewrite
    if (is_none_bias) {
      if (is_scalar_rhs) {
        // If the `constant_val` is scalar, we must the shape of filter
        // to properly broadcast the scalar to `{num_channels}` shape.

        // Get the number of channels if possible.
        auto filter_type = filter.getType().dyn_cast<RankedTensorType>();
        // Filter must be a `2D` tensor with `{num_channels, num_features}`
        // shape. The following check is rejecting unknown rank (-1).
        if (filter_type == nullptr || filter_type.getRank() != 2) {
          return failure();
        }
        int num_channels = filter_type.getShape()[0];

        // Create a zero tensor with shape {num_channels}, and the type need to
        // be the same as constant_val.
        // This is a way to gracefully handle scalar tensor. The Add will always
        // be constant-folded away regardless if `constant_val` is a scalar or
        // not.
        RankedTensorType type = RankedTensorType::get(
            {num_channels}, constant_val_type.getElementType());
        auto attr = rewriter.getZeroAttr(type);
        bias = rewriter.create<ConstantOp>(add_op.getLoc(), type, attr);
        auto none_af = rewriter.getStringAttr("NONE");
        bias =
            rewriter.create<AddOp>(add_op.getLoc(), bias, constant_val, none_af)
                .output();
      } else {
        // If there no pre-existing bias and the `constant_val` is 1D, simply
        // use `constant_val` as bias.
        bias = constant_val;
      }
    } else {
      auto none_af = rewriter.getStringAttr("NONE");
      bias =
          rewriter.create<AddOp>(add_op.getLoc(), bias, constant_val, none_af)
              .output();
    }

    auto fc = rewriter.create<TFL::FullyConnectedOp>(
        FusedLoc::get(fc_op.getContext(), {fc_op.getLoc(), add_op.getLoc()}),
        add_op.getType(),
        /*input=*/fc_op.input(),
        /*filter=*/filter,
        /*bias=*/bias,
        /*fused_activation_function=*/
        rewriter.getStringAttr(add_op.fused_activation_function()),
        /*weights_format=*/rewriter.getStringAttr(fc_op.weights_format()),
        /*keep_num_dims=*/rewriter.getBoolAttr(fc_op.keep_num_dims()));
    rewriter.replaceOp(add_op, fc.output());

    return success();
  }