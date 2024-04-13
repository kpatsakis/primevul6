static Variant HHVM_METHOD(SimpleXMLElementIterator, current) {
  return Native::data<SimpleXMLElementIterator>(this_)->sxe()->iter.data;
}