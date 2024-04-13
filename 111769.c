ShapeHandle InferenceContext::UnknownShapeOfRank(int64_t rank) {
  CHECK_LE(rank, kint32max) << "rank must be less than kint32max";
  if (rank == kUnknownRank) {
    return UnknownShape();
  }
  CHECK_GE(rank, 0) << "rank must not be negative";
  std::vector<DimensionHandle> dims(rank);
  for (int32_t i = 0; i < rank; ++i) {
    dims[i] = UnknownDim();
  }
  return MakeShape(dims);
}