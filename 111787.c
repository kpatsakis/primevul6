ShapeHandle InferenceContext::MakeShape(
    const std::vector<DimensionHandle>& dims) {
  return shape_manager_.MakeShape(dims);
}