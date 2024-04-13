static bool HHVM_METHOD(SimpleXMLElement, __isset, const Variant& name) {
  auto data = Native::data<SimpleXMLElement>(this_);
  return sxe_prop_dim_exists(data, name, false, true, false);
}