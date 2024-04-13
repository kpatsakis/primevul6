Status InferenceContext::Merge(ShapeHandle s0, ShapeHandle s1,
                               ShapeHandle* out) {
  if (s0.SameHandle(s1)) {
    *out = s0;
    return Status::OK();
  } else if (!RankKnown(s1)) {
    *out = s0;
    merged_shapes_.emplace_back(s0, s1);
    return Status::OK();
  } else if (!RankKnown(s0)) {
    *out = s1;
    merged_shapes_.emplace_back(s0, s1);
    return Status::OK();
  }

  const int32_t rank = Rank(s0);
  if (rank != Rank(s1)) {
    *out = nullptr;
    return errors::InvalidArgument("Shapes must be equal rank, but are ", rank,
                                   " and ", Rank(s1));
  }

  bool return_s0 = true;
  bool return_s1 = true;
  for (int i = 0; i < rank; ++i) {
    auto d0 = Dim(s0, i);
    auto d1 = Dim(s1, i);
    if (d0.SameHandle(d1)) continue;

    auto v0 = Value(d0);
    auto v1 = Value(d1);
    if (v0 == kUnknownDim) {
      if (v1 != kUnknownDim) {
        return_s0 = false;
      }
    } else if (v1 == kUnknownDim) {
      return_s1 = false;
    } else if (v0 != v1) {
      *out = nullptr;
      return errors::InvalidArgument(
          "Dimension ", i, " in both shapes must be equal, but are ", Value(d0),
          " and ", Value(d1), ". Shapes are ", DebugString(s0), " and ",
          DebugString(s1), ".");
    }
  }

  merged_shapes_.emplace_back(s0, s1);

  if (return_s0 || return_s1) {
    *out = return_s0 ? s0 : s1;
    return Status::OK();
  }

  // Merge dims.
  std::vector<DimensionHandle> dims(rank, nullptr);
  for (int i = 0; i < rank; ++i) {
    // Invariant for merge was checked earlier, so CHECK is ok.
    TF_CHECK_OK(Merge(Dim(s0, i), Dim(s1, i), &dims[i]));
  }

  Status s = ReturnCreatedShape(dims, out);
  if (s.ok()) {
    // Merge the new shape with s0. Since s0 and s1 are merged, this implies
    // that s1 and out are also merged.
    merged_shapes_.emplace_back(s0, *out);
  }
  return s;
}