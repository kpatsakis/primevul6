Status InferenceContext::WithRank(ShapeHandle shape, int64_t rank,
                                  ShapeHandle* out) {
  if (rank > kint32max) {
    return errors::InvalidArgument("Rank cannot exceed kint32max");
  }
  const int32_t existing = Rank(shape);
  if (existing == rank) {
    *out = shape;
    return Status::OK();
  }
  if (existing == kUnknownRank) {
    std::vector<DimensionHandle> dims;
    dims.reserve(rank);
    for (int i = 0; i < rank; ++i) {
      dims.push_back(UnknownDim());
    }
    ShapeHandle shp = shape_manager_.MakeShape(dims);
    return Merge(shape, shp, out);
  }
  *out = nullptr;

  return errors::InvalidArgument("Shape must be rank ", rank, " but is rank ",
                                 existing);
}