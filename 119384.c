  bool IsSupported(const NodeDef* node) const override {
    return IsAnySparseSegmentReduction(*node);
  }