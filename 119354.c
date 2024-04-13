  explicit FoldTransposeIntoMatMul(const GraphOptimizerContext& ctx,
                                   const ArithmeticOptimizerContext& ctx_ext)
      : ArithmeticOptimizerStage("FoldTransposeIntoMatMul", ctx, ctx_ext) {}