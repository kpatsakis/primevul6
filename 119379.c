  explicit RemoveStackSliceSameAxis(const GraphOptimizerContext& ctx,
                                    const ArithmeticOptimizerContext& ctx_ext)
      : ArithmeticOptimizerStage("RemoveStackStridedSliceSameAxis", ctx,
                                 ctx_ext) {}