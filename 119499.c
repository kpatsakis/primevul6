  explicit FuseSquaredDiffStage(const GraphOptimizerContext& ctx,
                                const ArithmeticOptimizerContext& ctx_ext)
      : ArithmeticOptimizerStage("FuseSquaredDiffStage", ctx, ctx_ext) {}