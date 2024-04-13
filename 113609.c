static bool HHVM_METHOD(SimpleXMLIterator, hasChildren) {
  auto children = HHVM_MN(SimpleXMLIterator, getChildren)(this_);
  if (!children.isObject()) {
    return false;
  }
  auto od = children.toObject().get();
  assert(od->instanceof(SimpleXMLElement_classof()));
  return HHVM_MN(SimpleXMLElement, count)(od) > 0;
}