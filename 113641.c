static Variant HHVM_METHOD(SimpleXMLElement, addChild,
                           const String& qname,
                           const String& value /* = null_string */,
                           const Variant& ns /* = null */) {
  if (qname.empty()) {
    raise_warning("Element name is required");
    return init_null();
  }

  auto data = Native::data<SimpleXMLElement>(this_);
  xmlNodePtr node = data->nodep();

  if (data->iter.type == SXE_ITER_ATTRLIST) {
    raise_warning("Cannot add element to attributes");
    return init_null();
  }

  node = php_sxe_get_first_node(data, node);

  if (node == nullptr) {
    raise_warning("Cannot add child. "
                  "Parent is not a permanent member of the XML tree");
    return init_null();
  }

  xmlChar* prefix = nullptr;
  xmlChar* localname = xmlSplitQName2((xmlChar*)qname.data(), &prefix);
  if (localname == nullptr) {
    localname = xmlStrdup((xmlChar*)qname.data());
  }

  xmlNodePtr newnode = xmlNewChild(node, nullptr, localname,
                                   (xmlChar*)value.data());

  xmlNsPtr nsptr = nullptr;
  if (!ns.isNull()) {
    const String& ns_ = ns.toString();
    if (ns_.empty()) {
      newnode->ns = nullptr;
      nsptr = xmlNewNs(newnode, (xmlChar*)ns_.data(), prefix);
    } else {
      nsptr = xmlSearchNsByHref(node->doc, node, (xmlChar*)ns_.data());
      if (nsptr == nullptr) {
        nsptr = xmlNewNs(newnode, (xmlChar*)ns_.data(), prefix);
      }
      newnode->ns = nsptr;
    }
  }

  Object ret = _node_as_zval(data, newnode, SXE_ITER_NONE, (char*)localname,
                             prefix, false);

  xmlFree(localname);
  if (prefix != nullptr) {
    xmlFree(prefix);
  }
  return ret;
}