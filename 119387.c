  explicit RemoveCastIntoSegmentReductionStage(
      const GraphOptimizerContext& ctx,
      const ArithmeticOptimizerContext& ctx_ext)
      : ArithmeticOptimizerStage("RemoveCastIntoSegmentReductionStage", ctx,
                                 ctx_ext) {}