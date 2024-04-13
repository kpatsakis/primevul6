bool InferenceContext::RelaxHandleShapesAndMergeTypes(
    const std::vector<ShapeAndType>& shapes_and_types,
    std::vector<ShapeAndType>* to_update) {
  if (shapes_and_types.size() != to_update->size()) {
    return false;
  }
  std::vector<ShapeAndType> new_values(shapes_and_types.size());
  for (int i = 0, end = shapes_and_types.size(); i < end; ++i) {
    const ShapeAndType& existing = (*to_update)[i];
    if (shapes_and_types[i].dtype == existing.dtype) {
      new_values[i].dtype = existing.dtype;
    } else {
      if (existing.dtype != DT_INVALID) {
        return false;
      } else {
        new_values[i].dtype = shapes_and_types[i].dtype;
      }
    }
    Relax(existing.shape, shapes_and_types[i].shape, &new_values[i].shape);
  }
  to_update->swap(new_values);
  return true;
}