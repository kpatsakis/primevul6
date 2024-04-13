static Variant HHVM_METHOD(SimpleXMLIterator, valid) {
  auto data = Native::data<SimpleXMLIterator>(this_);
  return !data->iter.data.isNull();
}