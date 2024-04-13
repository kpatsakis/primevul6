ShapeHandle InferenceContext::ShapeManager::MakeShape(
    const std::vector<DimensionHandle>& dims) {
  all_shapes_.push_back(new Shape(dims));
  return all_shapes_.back();
}