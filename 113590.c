  SimpleXMLElement() {
    auto obj = Native::object<SimpleXMLElement>(this);
    obj->setAttribute(ObjectData::HasPropEmpty);
    obj->setAttribute(ObjectData::CallToImpl);
  }