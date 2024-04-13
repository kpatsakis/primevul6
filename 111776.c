bool InferenceContext::MergeOutputHandleShapesAndTypes(
    int idx, const std::vector<ShapeAndType>& shapes_and_types) {
  if (output_handle_shapes_and_types_[idx] == nullptr) {
    output_handle_shapes_and_types_[idx].reset(
        new std::vector<ShapeAndType>(shapes_and_types));
    return true;
  }
  return MergeHandleShapesAndTypes(shapes_and_types,
                                   output_handle_shapes_and_types_[idx].get());
}