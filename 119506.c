  explicit RemoveLogicalNotStage(const GraphOptimizerContext& ctx,
                                 const ArithmeticOptimizerContext& ctx_ext)
      : ArithmeticOptimizerStage("RemoveLogicalNot", ctx, ctx_ext) {}