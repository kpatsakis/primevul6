static bool HHVM_METHOD(SimpleXMLElement, registerXPathNamespace,
                        const String& prefix, const String& ns) {
  auto data = Native::data<SimpleXMLElement>(this_);
  if (!data->xpath) {
    data->xpath = xmlXPathNewContext(data->docp());
  }

  if (xmlXPathRegisterNs(data->xpath,
                         (xmlChar*)prefix.data(),
                         (xmlChar*)ns.data()) != 0) {
    return false;
  }
  return true;
}