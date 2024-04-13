void OptimizePass::runOnFunction() {
  OwningRewritePatternList patterns(&getContext());
  auto *ctx = &getContext();
  auto func = getFunction();

  // Merge reshapes into fully connected ops before we start moving them past
  // binary ops.
  OwningRewritePatternList phase_0_patterns(&getContext());
  phase_0_patterns.insert<RemoveReshapeAfterFullyConnected,
                          RemoveReshapeBeforeFullyConnected>(ctx);
  (void)applyPatternsAndFoldGreedily(func, std::move(phase_0_patterns));

  // Potentially the binary ops might be fused together, like hard_swish, thus
  // we explore these potentially first and then fuse the binary ops with the
  // following ops in a second pattern match.
  TFL::populateWithGenerated(patterns);
  patterns.insert<FuseFullyConnectedAndAdd, FuseAddAndFullyConnected,
                  FuseFullyConnectedAndMul, FuseMulAndFullyConnected,
                  FuseFullyConnectedAndReluX<TFL::ReluOp, kRelu>,
                  FuseFullyConnectedAndReluX<TFL::Relu6Op, kRelu6>,
                  FuseFullyConnectedAndReluX<TFL::Relu1Op, kRelu1>>(ctx);
  if (enable_canonicalization_) AddCanonicalizationPatterns(ctx, &patterns);
  (void)applyPatternsAndFoldGreedily(func, std::move(patterns));

  // Fuse the binary ops with the following ops.
  OwningRewritePatternList phase_2_patterns(&getContext());
  TFL::populateWithGenerated(phase_2_patterns);
  phase_2_patterns.insert<
      ScalarizeSplatConstantForAdd, ScalarizeSplatConstantForSub,
      ScalarizeSplatConstantForMul, ScalarizeSplatConstantForDiv,
      FuseFullyConnectedAndAdd, FuseAddAndFullyConnected,
      FuseFullyConnectedAndMul, FuseMulAndFullyConnected,
      FuseFullyConnectedAndReluX<TFL::ReluOp, kRelu>,
      FuseFullyConnectedAndReluX<TFL::Relu6Op, kRelu6>,
      FuseFullyConnectedAndReluX<TFL::Relu1Op, kRelu1>,
      FuseBinaryOpToFollowingConv2D, FuseBinaryOpToFollowingDepthwiseConv2D,
      FuseBinaryOpToFollowingFullyConnected, FuseConv2DAndMulWithQDQs,
      FuseDepthwiseConv2DAndMulWithQDQs, ConvertTrivialTransposeOpToReshapeOp,
      RemoveReshapeAfterFullyConnected, RemoveReshapeBeforeFullyConnected,
      FuseUnpackAndConcatToReshape>(ctx);
  if (enable_canonicalization_)
    AddCanonicalizationPatterns(ctx, &phase_2_patterns);
  (void)applyPatternsAndFoldGreedily(func, std::move(phase_2_patterns));
}