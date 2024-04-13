  bool IsSupported(const NodeDef* node) const override {
    return IsLogicalNot(*node) && !IsInPreserveSet(*node);
  }