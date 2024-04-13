  bool IsSupported(const NodeDef* node) const override {
    return IsPack(*node) && NumNonControlInputs(*node) > 1 &&
           !IsInPreserveSet(*node);
  }