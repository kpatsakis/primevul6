Status InferenceContext::WithRankAtMost(ShapeHandle shape, int64_t rank,
                                        ShapeHandle* out) {
  if (rank > kint32max) {
    return errors::InvalidArgument("Rank cannot exceed kint32max");
  }
  const int32_t existing = Rank(shape);
  if (existing <= rank || existing == kUnknownRank) {
    *out = shape;
    return Status::OK();
  }
  *out = nullptr;
  return errors::InvalidArgument("Shape must be at most rank ", rank,
                                 " but is rank ", existing);
}