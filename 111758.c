Status InferenceContext::Add(DimensionHandle first, DimensionOrConstant second,
                             DimensionHandle* out) {
  const int64_t first_value = Value(first);
  const int64_t second_value = Value(second);
  // Special cases.
  if (first_value == 0) {
    *out = MakeDim(second);
  } else if (second_value == 0) {
    *out = first;
  } else if (first_value == kUnknownDim || second_value == kUnknownDim) {
    *out = UnknownDim();
  } else {
    // Invariant: Both values are known and positive. Still in run-time we can
    // get pair of values which cannot be store in output. Check below will
    // report error. We still need to avoid undefined behavior of signed
    // overflow and use unsigned addition.
    const int64_t sum = static_cast<uint64>(first_value) + second_value;
    if (sum < 0) {
      return errors::InvalidArgument("Dimension size overflow from adding ",
                                     first_value, " and ", second_value);
    }
    *out = MakeDim(sum);
  }
  return Status::OK();
}