static bool HHVM_METHOD(SimpleXMLElement, offsetExists,
                        const Variant& index) {
  auto data = Native::data<SimpleXMLElement>(this_);
  return sxe_prop_dim_exists(data, index, false, false, true);
}