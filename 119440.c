  bool IsSupported(const NodeDef* node) const override {
    return IsInvolution(*node);
  }