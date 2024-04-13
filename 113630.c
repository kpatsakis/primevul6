static void HHVM_METHOD(SimpleXMLElement, offsetSet,
                        const Variant& index, const Variant& newvalue) {
  auto data = Native::data<SimpleXMLElement>(this_);
  sxe_prop_dim_write(data, index, newvalue, false, true, nullptr);
}