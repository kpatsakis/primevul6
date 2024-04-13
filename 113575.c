static Array HHVM_METHOD(SimpleXMLElement, getDocNamespaces,
                         bool recursive /* = false */,
                         bool from_root /* = true */) {
  auto data = Native::data<SimpleXMLElement>(this_);
  xmlNodePtr node =
    from_root ? xmlDocGetRootElement(data->docp())
              : data->nodep();
  Array ret = Array::Create();
  sxe_add_registered_namespaces(data, node, recursive, ret);
  return ret;
}