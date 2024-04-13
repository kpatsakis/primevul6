  explicit RemoveRedundantBitcastStage(
      const GraphOptimizerContext& ctx,
      const ArithmeticOptimizerContext& ctx_ext)
      : ArithmeticOptimizerStage("RemoveRedundantBitcast", ctx, ctx_ext) {}