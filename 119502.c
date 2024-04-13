  bool IsSupported(const NodeDef* node) const override {
    // Note: div_no_nan(a, sqrt(b)) => mul_no_nan(a, rsqrt(b))
    // for b == 0 would result in a / Inf instead of 0.
    return IsAnyDiv(*node) && !IsDivNoNan(*node) && !IsFloorDiv(*node);
  }