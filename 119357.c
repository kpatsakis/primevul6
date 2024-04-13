  explicit ReplaceMulWithBroadcastByTile(
      const GraphOptimizerContext& ctx,
      const ArithmeticOptimizerContext& ctx_ext)
      : ArithmeticOptimizerStage("ReplaceMulWithBroadcastByTile", ctx,
                                 ctx_ext) {}