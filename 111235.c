TensorSliceReader::GetVariableToDataTypeMap() const {
  VarToDataTypeMap name_to_dtype;
  if (status().ok()) {
    for (auto& e : Tensors()) {
      name_to_dtype[e.first] = e.second->type();
    }
  }
  return name_to_dtype;
}