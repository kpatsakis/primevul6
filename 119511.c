  bool IsSupported(const NodeDef* node) const override {
    return IsBitcast(*node);
  }