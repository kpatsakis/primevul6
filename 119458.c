  explicit ArithmeticOptimizerStage(const string& name,
                                    const GraphOptimizerContext& ctx,
                                    const ArithmeticOptimizerContext ctx_ext)
      : GraphOptimizerStage("ArithmeticOptimizer", name, ctx),
        ctx_ext_(ctx_ext) {}