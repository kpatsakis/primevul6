  explicit RemoveIdentityTranspose(const GraphOptimizerContext& ctx,
                                   const ArithmeticOptimizerContext& ctx_ext)
      : ArithmeticOptimizerStage("RemoveIdentityTranspose", ctx, ctx_ext) {}