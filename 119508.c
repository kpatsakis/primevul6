  bool IsSupported(const NodeDef* node) const override {
    return node->input_size() == 1 && IsIdempotent(*node) &&
           !IsInPreserveSet(*node);
  }