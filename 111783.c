ShapeHandle InferenceContext::ShapeManager::UnknownShape() {
  all_shapes_.push_back(new Shape());
  return all_shapes_.back();
}