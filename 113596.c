const Class* SimpleXMLIterator_classof() {
  static auto cls = Unit::lookupClass(s_SimpleXMLIterator.get());
  return cls;
}