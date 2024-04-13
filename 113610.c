static Variant HHVM_METHOD(SimpleXMLElement, __set,
                           const Variant& name, const Variant& value) {
  auto data = Native::data<SimpleXMLElement>(this_);
  return sxe_prop_dim_write(data, name, value, true, false, nullptr);
}