  explicit LogSoftmaxStage(const GraphOptimizerContext& ctx,
                           const ArithmeticOptimizerContext& ctx_ext)
      : ArithmeticOptimizerStage("LogSoftmaxStage", ctx, ctx_ext) {}