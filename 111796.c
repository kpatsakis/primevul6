Status InferenceContext::Subtract(DimensionHandle first,
                                  DimensionOrConstant second,
                                  DimensionHandle* out) {
  const int64_t first_value = Value(first);
  const int64_t second_value = Value(second);
  // Special cases.
  if (second_value == 0) {
    *out = first;
  } else if (first_value == kUnknownDim || second_value == kUnknownDim) {
    *out = UnknownDim();
  } else {
    // Invariant: Both values are known, first_value is non-negative, and
    // second_value is positive.
    if (first_value < second_value) {
      return errors::InvalidArgument(
          "Negative dimension size caused by subtracting ", second_value,
          " from ", first_value);
    }
    *out = MakeDim(first_value - second_value);
  }
  return Status::OK();
}