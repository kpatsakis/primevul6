  bool IsSupported(const NodeDef* node) const override {
    return IsAnyMax(*node) || IsAnyMin(*node) || IsAnyMaxPool(*node) ||
           IsArgMax(*node) || IsArgMin(*node);
  }