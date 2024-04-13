static Variant HHVM_METHOD(SimpleXMLElementIterator, rewind) {
  auto sxe = Native::data<SimpleXMLElementIterator>(this_)->sxe();
  php_sxe_reset_iterator(sxe, true);
  return init_null();
}