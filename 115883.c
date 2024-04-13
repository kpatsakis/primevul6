std::unique_ptr<OperationPass<FuncOp>> CreateOptimizePass(
    bool enable_canonicalization) {
  return std::make_unique<OptimizePass>(enable_canonicalization);
}