TensorSliceReader::VarToShapeMap TensorSliceReader::GetVariableToShapeMap()
    const {
  VarToShapeMap name_to_shape;
  if (status().ok()) {
    for (auto& e : Tensors()) {
      name_to_shape[e.first] = e.second->shape();
    }
  }
  return name_to_shape;
}