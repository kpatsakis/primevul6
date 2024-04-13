ElementsAttr ExpandTo4DForConvImpl(Attribute a, bool is_depthwise) {
  auto elements = a.dyn_cast<DenseElementsAttr>();
  auto shape = elements.getType().getShape();
  if (!shape.empty()) {
    // Checks that elements are essentially 1d.
    assert(elements.getNumElements() == shape.back());
  }
  std::vector<int64_t> shape_data = {1, 1, 1, 1};
  const int vector_length = elements.getNumElements();
  if (is_depthwise)
    shape_data[3] = vector_length;
  else
    shape_data[0] = vector_length;
  auto new_shape =
      RankedTensorType::get(shape_data, elements.getType().getElementType());
  return elements.reshape(new_shape);
}