static Array HHVM_METHOD(SimpleXMLElement, getNamespaces,
                         bool recursive /* = false */) {
  auto data = Native::data<SimpleXMLElement>(this_);
  Array ret = Array::Create();
  xmlNodePtr node = data->nodep();
  node = php_sxe_get_first_node(data, node);
  if (node) {
    if (node->type == XML_ELEMENT_NODE) {
      sxe_add_namespaces(data, node, recursive, ret);
    } else if (node->type == XML_ATTRIBUTE_NODE && node->ns) {
      sxe_add_namespace_name(ret, node->ns);
    }
  }
  return ret;
}