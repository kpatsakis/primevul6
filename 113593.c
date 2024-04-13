static Variant HHVM_METHOD(SimpleXMLElement, children,
                           const String& ns = empty_string(),
                           bool is_prefix = false) {
  auto data = Native::data<SimpleXMLElement>(this_);
  if (data->iter.type == SXE_ITER_ATTRLIST) {
    return init_null(); /* attributes don't have attributes */
  }

  xmlNodePtr node = data->nodep();
  node = php_sxe_get_first_node(data, node);
  return _node_as_zval(data, node, SXE_ITER_CHILD, nullptr,
                       (xmlChar*)ns.data(), is_prefix);
}