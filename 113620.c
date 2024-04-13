static Variant HHVM_METHOD(SimpleXMLIterator, current) {
  auto data = Native::data<SimpleXMLIterator>(this_);
  return data->iter.data;
}