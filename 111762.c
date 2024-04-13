Status InferenceContext::Divide(DimensionHandle dividend,
                                DimensionOrConstant divisor,
                                bool evenly_divisible, DimensionHandle* out) {
  const int64_t divisor_value = Value(divisor);
  if (divisor_value == 1) {
    *out = dividend;
  } else if (!ValueKnown(dividend) ||
             (divisor.dim.IsSet() && !ValueKnown(divisor.dim))) {
    *out = UnknownDim();
  } else {
    const int64_t v = Value(dividend);
    if (divisor_value <= 0) {
      return errors::InvalidArgument("Divisor must be positive but is ",
                                     divisor_value);
    }
    if (evenly_divisible && (v % divisor_value) != 0) {
      return errors::InvalidArgument(
          "Dimension size must be evenly divisible by ", divisor_value,
          " but is ", v);
    }
    *out = MakeDim(v / divisor_value);
  }
  return Status::OK();
}