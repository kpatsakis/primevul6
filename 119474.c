  bool IsSupported(const NodeDef* node) const override {
    return IsReshape(*node) && !IsInPreserveSet(*node);
  }