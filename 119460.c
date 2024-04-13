  explicit ReduceUpsamplingDims(const GraphOptimizerContext& ctx,
                                const ArithmeticOptimizerContext& ctx_ext)
      : ArithmeticOptimizerStage("ReduceUpsamplingDims", ctx, ctx_ext) {}