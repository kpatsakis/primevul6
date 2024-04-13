  bool IsSupported(const NodeDef* node) const override {
    if (!node || node->input_size() < 2) {
      // Invalid node
      return false;
    }

    return IsAnyMul(*node) && node->input(0) == node->input(1);
  }