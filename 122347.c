bool FastAreAttrValuesEqual(const AttrValue& a, const AttrValue& b) {
  return AreAttrValuesEqual(a, b, FastAreTensorProtosEqual);
}