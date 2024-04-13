Status InferenceContext::Subshape(ShapeHandle s, int64_t start, int64_t end,
                                  ShapeHandle* out) {
  return Subshape(s, start, end, 1 /* stride */, out);
}