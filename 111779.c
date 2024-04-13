DimensionHandle InferenceContext::NumElements(ShapeHandle s) {
  const auto rank = Rank(s);
  if (rank == kUnknownRank) return UnknownDim();
  bool found_unknown = false;
  int64_t size = 1;
  for (int i = 0; i < rank; ++i) {
    int64_t dim_val = Value(Dim(s, i));
    if (dim_val == kUnknownDim) {
      found_unknown = true;
    } else if (dim_val == 0) {
      return MakeDim(0);
    } else {
      size *= dim_val;
    }
  }
  if (found_unknown) {
    return UnknownDim();
  } else {
    return MakeDim(size);
  }
}