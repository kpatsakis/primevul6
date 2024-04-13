static String HHVM_METHOD(SimpleXMLElement, getName) {
  auto data = Native::data<SimpleXMLElement>(this_);
  xmlNodePtr node = data->nodep();
  node = php_sxe_get_first_node(data, node);
  if (node) {
    return String((char*)node->name);
  }
  return empty_string();
}