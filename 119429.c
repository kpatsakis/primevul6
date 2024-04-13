  bool IsSupported(const NodeDef* node) const override {
    return IsTranspose(*node) || IsConjugateTranspose(*node);
  }