  explicit ReorderCastLikeAndValuePreserving(
      const GraphOptimizerContext& ctx,
      const ArithmeticOptimizerContext& ctx_ext)
      : ArithmeticOptimizerStage("ReorderCastLikeAndValuePreserving", ctx,
                                 ctx_ext) {}