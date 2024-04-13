bool AreAttrValuesEqual(const AttrValue& a, const AttrValue& b) {
  return AreAttrValuesEqual(a, b, AreTensorProtosEqual);
}