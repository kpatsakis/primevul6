  explicit SimplifyAggregation(const GraphOptimizerContext& ctx,
                               const ArithmeticOptimizerContext& ctx_ext)
      : ArithmeticOptimizerStage("SimplifyAggregation", ctx, ctx_ext) {}