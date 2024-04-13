Status InferenceContext::ReplaceDim(ShapeHandle s, int64_t dim_index_in,
                                    DimensionHandle new_dim, ShapeHandle* out) {
  if (!RankKnown(s)) {
    return ReturnUnknownShape(out);
  }
  int64_t dim_index = dim_index_in;
  if (dim_index < 0) {
    dim_index = s->dims_.size() + dim_index;
  }
  if (!FastBoundsCheck(dim_index, s->dims_.size())) {
    *out = nullptr;
    return errors::InvalidArgument("Out of range dim_index ", dim_index_in,
                                   " for shape with ", s->dims_.size(),
                                   " dimensions");
  }
  std::vector<DimensionHandle> dims(s->dims_);
  dims[dim_index] = new_dim;
  return ReturnCreatedShape(dims, out);
}