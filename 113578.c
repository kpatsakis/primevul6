static Variant HHVM_METHOD(SimpleXMLElement, __get, const Variant& name) {
  auto data = Native::data<SimpleXMLElement>(this_);
  return sxe_prop_dim_read(data, name, true, false);
}