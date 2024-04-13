  bool IsSupported(const NodeDef* node) const override {
    return IsMul(*node) && !IsInPreserveSet(*node);
  }