static Variant HHVM_METHOD(SimpleXMLElementIterator, key) {
  auto sxe = Native::data<SimpleXMLElementIterator>(this_)->sxe();
  Object curobj = sxe->iter.data;
  xmlNodePtr curnode = curobj.isNull()
    ? nullptr
    : Native::data<SimpleXMLElement>(curobj.get())->nodep();
  if (curnode) {
    return String((char*)curnode->name);
  } else {
    return init_null();
  }
}