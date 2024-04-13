  explicit HoistCommonFactorOutOfAggregation(
      const GraphOptimizerContext& ctx,
      const ArithmeticOptimizerContext& ctx_ext)
      : ArithmeticOptimizerStage("HoistCommonFactor", ctx, ctx_ext) {}