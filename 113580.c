static Variant HHVM_METHOD(SimpleXMLIterator, next) {
  auto data = Native::data<SimpleXMLIterator>(this_);
  php_sxe_move_forward_iterator(data);
  return init_null();
}