  explicit ConvertPowStage(const GraphOptimizerContext& ctx,
                           const ArithmeticOptimizerContext& ctx_ext)
      : ArithmeticOptimizerStage("ConvertPow", ctx, ctx_ext) {}