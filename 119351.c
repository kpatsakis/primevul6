bool ValuesFromConstNode(const NodeDef& node, std::vector<T>* values) {
  if (node.op() != "Const") {
    return false;
  }

  if (node.attr().count("dtype") == 0 || node.attr().count("value") == 0 ||
      node.attr().at("dtype").type() != DataTypeToEnum<T>::value) {
    return false;
  }

  // TensorProto represents the content of the tensor in either <type>_val or
  // tensor_content.
  const TensorProto& tensor = node.attr().at("value").tensor();
  typename checkpoint::SaveTypeTraits<T>::RepeatedField* tensor_values =
      checkpoint::MutableTensorProtoData<T>(const_cast<TensorProto*>(&tensor));

  if (!tensor_values->empty() && tensor.has_tensor_shape()) {
    // When tensor_shape is set, theoretically the representation of the data
    // could be compressed. So, before copying values to the returned vector,
    // make sure no compression happens.
    const TensorShapeProto& shape = tensor.tensor_shape();
    if (shape.dim_size() == 1 && shape.dim(0).size() == tensor_values->size()) {
      values->insert(values->end(), tensor_values->begin(),
                     tensor_values->end());
      return true;
    }
  }

  const auto tensor_content_size = tensor.tensor_content().size();
  if (tensor_content_size > 0) {
    CHECK_EQ(0, tensor_content_size % sizeof(T))
        << "tensor_content_size (" << tensor_content_size
        << ") is not a multiple of " << sizeof(T);
    values->resize(tensor_content_size / sizeof(T));
    port::CopyToArray(tensor.tensor_content(),
                      reinterpret_cast<char*>(values->data()));
    return true;
  }

  return false;
}