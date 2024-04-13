  bool IsSupported(const NodeDef* node) const override {
    return (IsValuePreserving(*node) || IsCastLike(*node)) &&
           !IsCheckNumerics(*node) && NodeIsOnCpuOrGpu(node) &&
           !IsControlFlow(*node) && !IsInPreserveSet(*node);
  }