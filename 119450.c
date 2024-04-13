  bool IsSupported(const NodeDef* node) const override {
    return (IsAdd(*node) || IsSub(*node)) && !IsInPreserveSet(*node);
  }