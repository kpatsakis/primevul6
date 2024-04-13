bool IsLastElementEqualsOne(Attribute attr) {
  const auto ints = attr.dyn_cast_or_null<DenseIntElementsAttr>();
  if (!ints) return false;
  if (ints.empty()) return false;
  const auto last_element_index = ints.getNumElements() - 1;
  const auto iterator = ints.getIntValues().begin();
  const APInt last_element = iterator[last_element_index];
  return last_element == 1;
}