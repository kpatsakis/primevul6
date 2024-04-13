void InferenceContext::Relax(ShapeHandle s_old, ShapeHandle s_new,
                             ShapeHandle* out) {
  if (s_old.SameHandle(s_new)) {
    *out = s_old;
    return;
  } else if (!RankKnown(s_new) || !s_old.IsSet()) {
    ForgetMerges();
    *out = s_new;
    return;
  }

  const int32_t rank = Rank(s_old);
  if (rank != Rank(s_new)) {
    ForgetMerges();
    *out = UnknownShape();
    return;
  }

  bool return_s_old = true;
  for (int i = 0; i < rank; ++i) {
    auto d0 = Dim(s_old, i);
    auto d1 = Dim(s_new, i);
    if (d0.SameHandle(d1)) continue;

    auto v0 = Value(d0);
    auto v1 = Value(d1);
    if (v0 == kUnknownDim || v1 == kUnknownDim || v0 != v1) {
      return_s_old = false;
      break;
    }
  }
  if (return_s_old) {
    *out = s_old;
    return;
  }

  // Relax dims.
  std::vector<DimensionHandle> dims(rank);
  for (int i = 0; i < rank; ++i) {
    Relax(Dim(s_old, i), Dim(s_new, i), &dims[i]);
  }
  ForgetMerges();
  *out = MakeShape(dims);
}