  explicit MinimizeBroadcasts(const GraphOptimizerContext& ctx,
                              const ArithmeticOptimizerContext& ctx_ext)
      : ArithmeticNodesGroupOptimizerStage("MinimizeBroadcasts", ctx, ctx_ext) {
  }