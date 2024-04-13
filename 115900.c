void AddCanonicalizationPatterns(MLIRContext *context,
                                 OwningRewritePatternList *patterns) {
  for (auto *op : context->getRegisteredOperations())
    op->getCanonicalizationPatterns(*patterns, context);
}