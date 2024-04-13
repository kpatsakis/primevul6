static Variant HHVM_METHOD(SimpleXMLElementIterator, next) {
  auto sxe = Native::data<SimpleXMLElementIterator>(this_)->sxe();
  php_sxe_move_forward_iterator(sxe);
  return init_null();
}