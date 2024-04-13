  explicit RemoveNegationStage(const GraphOptimizerContext& ctx,
                               const ArithmeticOptimizerContext& ctx_ext)
      : ArithmeticOptimizerStage("RemoveNegation", ctx, ctx_ext) {}