static Variant HHVM_METHOD(SimpleXMLIterator, rewind) {
  auto data = Native::data<SimpleXMLIterator>(this_);
  php_sxe_reset_iterator(data, true);
  return init_null();
}