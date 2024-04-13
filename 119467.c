  explicit OptimizeMaxOrMinOfMonotonicStage(
      const GraphOptimizerContext& ctx,
      const ArithmeticOptimizerContext& ctx_ext)
      : ArithmeticOptimizerStage("OptimizeMaxOrMinOfMonotonicStage", ctx,
                                 ctx_ext) {}