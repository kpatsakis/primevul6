  explicit RemoveRedundantCastStage(const GraphOptimizerContext& ctx,
                                    const ArithmeticOptimizerContext& ctx_ext)
      : ArithmeticOptimizerStage("RemoveRedundantCast", ctx, ctx_ext) {}