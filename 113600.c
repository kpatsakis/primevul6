static Variant HHVM_METHOD(SimpleXMLElement, __unset, const Variant& name) {
  auto data = Native::data<SimpleXMLElement>(this_);
  sxe_prop_dim_delete(data, name, true, false);
  return init_null();
}