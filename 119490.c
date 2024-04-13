  explicit ReplacePackWithTileReshape(const GraphOptimizerContext& ctx,
                                      const ArithmeticOptimizerContext& ctx_ext)
      : ArithmeticOptimizerStage("ReplacePackWithTileReshape", ctx, ctx_ext) {}