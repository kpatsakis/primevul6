  explicit FoldMultiplyIntoConv(const GraphOptimizerContext& ctx,
                                const ArithmeticOptimizerContext& ctx_ext)
      : ArithmeticOptimizerStage("FoldMultiplyIntoConv", ctx, ctx_ext) {}