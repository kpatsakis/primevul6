static Variant HHVM_METHOD(SimpleXMLIterator, key) {
  auto data = Native::data<SimpleXMLIterator>(this_);
  Object curobj = data->iter.data;
  if (curobj.isNull()) {
    return init_null();
  }

  assert(curobj->instanceof(SimpleXMLElement_classof()));
  auto curnode = Native::data<SimpleXMLElement>(curobj.get())->nodep();
  return String((char*)curnode->name);
}