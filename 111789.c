InferenceContext::ShapeManager::~ShapeManager() {
  for (auto* s : all_shapes_) delete s;
  for (auto* d : all_dims_) delete d;
}