  bool IsSupported(const NodeDef* node) const override {
    return (IsReshape(*node) || IsBroadcastTo(*node)) &&
           !IsInPreserveSet(*node);
  }