const Class* SimpleXMLElement_classof() {
  static auto cls = Unit::lookupClass(s_SimpleXMLElement.get());
  return cls;
}