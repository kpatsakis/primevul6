  explicit RemoveIdempotentStage(const GraphOptimizerContext& ctx,
                                 const ArithmeticOptimizerContext& ctx_ext)
      : ArithmeticOptimizerStage("RemoveIdempotent", ctx, ctx_ext) {}