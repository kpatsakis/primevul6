  explicit SimplifyEmbeddingLookupStage(
      const GraphOptimizerContext& ctx,
      const ArithmeticOptimizerContext& ctx_ext)
      : ArithmeticOptimizerStage("SimplifyEmbeddingLookupStage", ctx, ctx_ext) {
  }