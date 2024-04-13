  bool IsSupported(const NodeDef* node) const override {
    return IsConv2D(*node) || IsConv3D(*node);
  }