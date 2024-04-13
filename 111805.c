Status InferenceContext::Max(DimensionHandle first, DimensionOrConstant second,
                             DimensionHandle* out) {
  const int64_t first_value = Value(first);
  const int64_t second_value = Value(second);
  if (first_value == kUnknownDim || second_value == kUnknownDim) {
    *out = UnknownDim();
  } else {
    if (first_value >= second_value) {
      *out = first;
    } else {
      *out = MakeDim(second);
    }
  }
  return Status::OK();
}