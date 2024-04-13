Status InferenceContext::MergePrefix(ShapeHandle s, ShapeHandle prefix,
                                     ShapeHandle* s_out,
                                     ShapeHandle* prefix_out) {
  *s_out = *prefix_out = nullptr;
  if (!RankKnown(prefix) || !RankKnown(s)) {
    *s_out = s;
    *prefix_out = prefix;
    return Status::OK();
  }
  const int32_t rank = Rank(prefix);
  TF_RETURN_IF_ERROR(WithRankAtLeast(s, rank, &s));

  // Merge the prefix dims and create the new output shapes.
  const int32_t rank_s = Rank(s);
  std::vector<DimensionHandle> dims;
  dims.reserve(std::max(rank, rank_s));
  dims.resize(rank);
  for (int i = 0; i < rank; ++i) {
    TF_RETURN_IF_ERROR(Merge(Dim(s, i), Dim(prefix, i), &dims[i]));
  }
  *prefix_out = MakeShape(dims);
  for (int i = rank; i < rank_s; ++i) dims.push_back(Dim(s, i));
  *s_out = MakeShape(dims);
  return Status::OK();
}