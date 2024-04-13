static Variant HHVM_METHOD(SimpleXMLIterator, getChildren) {
  auto data = Native::data<SimpleXMLIterator>(this_);
  auto current = data->iter.data;
  if (current.isNull()) {
    return init_null();
  }
  assert(current->instanceof(SimpleXMLElement_classof()));
  return HHVM_MN(SimpleXMLElement, children)(current.get());
}