  bool IsSupported(const NodeDef* node) const override {
    return IsConj(*node) || IsTranspose(*node) || IsConjugateTranspose(*node);
  }