Status InferenceContext::WithValue(DimensionHandle dim, int64_t value,
                                   DimensionHandle* out) {
  const int64_t existing = Value(dim);
  if (existing == value) {
    *out = dim;
    return Status::OK();
  }
  if (existing == kUnknownDim) {
    DimensionHandle d = MakeDim(value);
    return Merge(dim, d, out);
  }
  *out = nullptr;
  return errors::InvalidArgument("Dimension must be ", value, " but is ",
                                 existing);
}