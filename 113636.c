static Variant HHVM_METHOD(SimpleXMLElementIterator, valid) {
  auto sxe = Native::data<SimpleXMLElementIterator>(this_)->sxe();
  return !sxe->iter.data.isNull();
}