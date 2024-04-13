Attribute GetNumElementsOrOne(Attribute attr) {
  const auto dense_attr = attr.dyn_cast_or_null<DenseElementsAttr>();
  int32_t num_elements = dense_attr ? dense_attr.getNumElements() : 1;

  OpBuilder builder(attr.getContext());

  return DenseIntElementsAttr::get(
      RankedTensorType::get({}, builder.getI32Type()),
      {llvm::APInt(32, num_elements, true)});
}