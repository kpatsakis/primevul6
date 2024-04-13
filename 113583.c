static Variant HHVM_METHOD(SimpleXMLElement, offsetGet,
                           const Variant& index) {
  auto data = Native::data<SimpleXMLElement>(this_);
  return sxe_prop_dim_read(data, index, false, true);
}