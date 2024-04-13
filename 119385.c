  explicit FoldConjugateIntoTranspose(const GraphOptimizerContext& ctx,
                                      const ArithmeticOptimizerContext& ctx_ext)
      : ArithmeticOptimizerStage("FoldConjugateIntoTranspose", ctx, ctx_ext) {}