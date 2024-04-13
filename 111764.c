bool InferenceContext::MergeInputHandleShapesAndTypes(
    int idx, const std::vector<ShapeAndType>& shapes_and_types) {
  if (input_handle_shapes_and_types_[idx] == nullptr) {
    input_handle_shapes_and_types_[idx].reset(
        new std::vector<ShapeAndType>(shapes_and_types));
    return true;
  }
  return MergeHandleShapesAndTypes(shapes_and_types,
                                   input_handle_shapes_and_types_[idx].get());
}