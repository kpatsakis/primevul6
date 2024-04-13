  bool IsSupported(const NodeDef* node) const override {
    return IsSquare(*node);
  }