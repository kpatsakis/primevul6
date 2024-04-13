static void HHVM_METHOD(SimpleXMLElement, offsetUnset,
                        const Variant& index) {
  auto data = Native::data<SimpleXMLElement>(this_);
  sxe_prop_dim_delete(data, index, false, true);
}