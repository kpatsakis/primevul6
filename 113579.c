static void HHVM_METHOD(SimpleXMLElement, addAttribute,
                        const String& qname,
                        const String& value /* = null_string */,
                        const String& ns /* = null_string */) {
  if (qname.size() == 0) {
    raise_warning("Attribute name is required");
    return;
  }

  auto data = Native::data<SimpleXMLElement>(this_);
  xmlNodePtr node = data->nodep();
  node = php_sxe_get_first_node(data, node);

  if (node && node->type != XML_ELEMENT_NODE) {
    node = node->parent;
  }

  if (node == nullptr) {
    raise_warning("Unable to locate parent Element");
    return;
  }

  xmlChar* prefix = nullptr;
  xmlChar* localname = xmlSplitQName2((xmlChar*)qname.data(), &prefix);
  if (localname == nullptr) {
    if (ns.size() > 0) {
      if (prefix != nullptr) {
        xmlFree(prefix);
      }
      raise_warning("Attribute requires prefix for namespace");
      return;
    }
    localname = xmlStrdup((xmlChar*)qname.data());
  }

  xmlAttrPtr attrp = xmlHasNsProp(node, localname, (xmlChar*)ns.data());
  if (attrp != nullptr && attrp->type != XML_ATTRIBUTE_DECL) {
    xmlFree(localname);
    if (prefix != nullptr) {
      xmlFree(prefix);
    }
    raise_warning("Attribute already exists");
    return;
  }

  xmlNsPtr nsptr = nullptr;
  if (ns.size()) {
    nsptr = xmlSearchNsByHref(node->doc, node, (xmlChar*)ns.data());
    if (nsptr == nullptr) {
      nsptr = xmlNewNs(node, (xmlChar*)ns.data(), prefix);
    }
  }

  attrp = xmlNewNsProp(node, nsptr, localname, (xmlChar*)value.data());

  xmlFree(localname);
  if (prefix != nullptr) {
    xmlFree(prefix);
  }
}