  explicit RemoveRedundantReshapeOrBroadcastTo(
      const GraphOptimizerContext& ctx,
      const ArithmeticOptimizerContext& ctx_ext)
      : ArithmeticOptimizerStage("RemoveRedundantReshapeOrBroadcastTo", ctx,
                                 ctx_ext) {}