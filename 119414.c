  bool IsSupported(const NodeDef* node) const override {
    return IsCast(*node) && !IsInPreserveSet(*node);
  }