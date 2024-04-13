  bool IsSupported(const NodeDef* node) const override {
    return IsAnyMatMul(*node) && !IsInPreserveSet(*node);
  }