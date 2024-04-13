  explicit ConvertLog1pStage(const GraphOptimizerContext& ctx,
                             const ArithmeticOptimizerContext& ctx_ext)
      : ArithmeticOptimizerStage("ConvertLog1p", ctx, ctx_ext) {}