  bool IsSupported(const NodeDef* node) const override {
    return (IsStridedSlice(*node) || IsSlice(*node)) && !IsInPreserveSet(*node);
  }