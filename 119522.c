  explicit ReorderReshapeAroundUnary(const GraphOptimizerContext& ctx,
                                     const ArithmeticOptimizerContext& ctx_ext)
      : ArithmeticOptimizerStage("ReorderReshapeAroundUnary", ctx, ctx_ext) {}