Status InferenceContext::Subshape(ShapeHandle s, int64_t start, int64_t end,
                                  int64_t stride, ShapeHandle* out) {
  int64_t start_in = start;
  int64_t end_in = end;

  const int32_t rank = Rank(s);
  if (start == 0 && stride == 1 &&
      ((RankKnown(s) && end >= rank) ||
       end == std::numeric_limits<int64_t>::max())) {
    *out = s;
    return Status::OK();
  }
  if (!RankKnown(s)) {
    return ReturnUnknownShape(out);
  }

  if (start > rank) start = rank;
  if (end > rank) end = rank;

  if (stride < 0 && start == rank) --start;

  if (start < 0) {
    start = rank + start;
    if (start < 0) {
      *out = nullptr;
      return errors::InvalidArgument("Subshape start out of bounds: ", start_in,
                                     ", for shape with rank ", rank);
    }
  }

  if (end < 0) {
    end = rank + end;
    if (end < 0) {
      *out = nullptr;
      return errors::InvalidArgument("Subshape end out of bounds: ", end_in,
                                     ", for shape with rank ", rank);
    }
  }
  if (stride > 0 && start > end) {
    *out = nullptr;
    return errors::InvalidArgument(
        "Subshape must have computed start <= end, but is ", start, " and ",
        end, " (computed from start ", start_in, " and end ", end_in,
        " over shape with rank ", rank, ")");
  } else if (stride < 0 && start < end) {
    *out = nullptr;
    return errors::InvalidArgument(
        "Subshape must have computed start >= end since stride is negative, "
        "but is ",
        start, " and ", end, " (computed from start ", start_in, " and end ",
        end_in, " over shape with rank ", rank, " and stride", stride, ")");
  }

  std::vector<DimensionHandle> dims;
  for (int i = start; stride > 0 ? i < end : i > end; i += stride) {
    dims.push_back(Dim(s, i));
  }
  return ReturnCreatedShape(dims, out);
}