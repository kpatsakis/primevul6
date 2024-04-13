static const Class* class_from_name(const String& class_name,
                                    const char* callee) {
  const Class* cls;
  if (!class_name.empty()) {
    cls = Unit::loadClass(class_name.get());
    if (!cls) {
      throw_invalid_argument("class not found: %s", class_name.data());
      return nullptr;
    }
    if (!cls->classof(SimpleXMLElement_classof())) {
      throw_invalid_argument(
        "%s() expects parameter 2 to be a class name "
        "derived from SimpleXMLElement, '%s' given",
        callee,
        class_name.data());
      return nullptr;
    }
  } else {
    cls = SimpleXMLElement_classof();
  }
  return cls;
}