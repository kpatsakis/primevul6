const Class* SimpleXMLElementIterator_classof() {
  static auto cls = Unit::lookupClass(s_SimpleXMLElementIterator.get());
  return cls;
}