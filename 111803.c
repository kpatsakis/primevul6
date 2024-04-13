Status InferenceContext::Subshape(ShapeHandle s, int64_t start,
                                  ShapeHandle* out) {
  return Subshape(s, start, std::numeric_limits<int64_t>::max() /* end */, out);
}