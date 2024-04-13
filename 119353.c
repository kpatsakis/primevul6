  explicit AddOpsRewriteStage(const GraphOptimizerContext& ctx,
                              const ArithmeticOptimizerContext& ctx_ext)
      : ArithmeticNodesGroupOptimizerStage("AddOpsRewrite", ctx, ctx_ext) {}