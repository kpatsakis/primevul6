  explicit SqrtDivToRsqrtMulStage(const GraphOptimizerContext& ctx,
                                  const ArithmeticOptimizerContext& ctx_ext)
      : ArithmeticOptimizerStage("SqrtDivToRsqrtMul", ctx, ctx_ext) {}