Status InferenceContext::Concatenate(ShapeHandle s1, ShapeHandle s2,
                                     ShapeHandle* out) {
  if (!RankKnown(s1) || !RankKnown(s2)) {
    return ReturnUnknownShape(out);
  }
  const int32_t s1_rank = Rank(s1);
  const int32_t s2_rank = Rank(s2);
  const int32_t rank = s1_rank + s2_rank;
  std::vector<DimensionHandle> dims;
  dims.reserve(rank);
  for (int i = 0; i < s1_rank; ++i) dims.push_back(Dim(s1, i));
  for (int i = 0; i < s2_rank; ++i) dims.push_back(Dim(s2, i));
  return ReturnCreatedShape(dims, out);
}