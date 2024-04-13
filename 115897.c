bool IsOneHotIndexAttribute(Attribute attr) {
  const auto dense_attr = attr.dyn_cast_or_null<DenseIntElementsAttr>();
  if (!dense_attr) {
    return false;
  }
  auto index_type = dense_attr.getType();
  const auto index_elem_bits = index_type.getElementTypeBitWidth();
  if (index_elem_bits != 32 && index_elem_bits != 64) {
    return false;
  }
  if (index_type.getRank() != 1) {
    return false;
  }
  const auto elems = dense_attr.getIntValues().begin();
  for (int i = 0; i < dense_attr.getNumElements(); ++i) {
    if (i != elems[i]) {
      return false;
    }
  }
  return true;
}