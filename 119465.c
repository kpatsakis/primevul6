  bool IsSupported(const NodeDef* node) const override {
    return IsAggregate(*node) && NumNonControlInputs(*node) > 1 &&
           !IsRewritten(node);
  }