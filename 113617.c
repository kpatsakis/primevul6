static Variant _get_base_node_value(SimpleXMLElement* sxe_ref,
                                    xmlNodePtr node, xmlChar* nsprefix,
                                    bool isprefix) {
  if (node->children &&
      node->children->type == XML_TEXT_NODE &&
      !xmlIsBlankNode(node->children)) {
    xmlChar* contents = xmlNodeListGetString(node->doc, node->children, 1);
    if (contents) {
      String obj = String((char*)contents);
      xmlFree(contents);
      return obj;
    }
  } else {
    auto sxeRefObj = Native::object<SimpleXMLElement>(sxe_ref);
    Object obj = create_object(sxeRefObj->getClassName(), Array(), false);
    auto subnode = Native::data<SimpleXMLElement>(obj.get());
    if (nsprefix && *nsprefix) {
      subnode->iter.nsprefix = xmlStrdup((xmlChar*)nsprefix);
      subnode->iter.isprefix = isprefix;
    }
    subnode->node = libxml_register_node(node);
    return obj;
  }
  return init_null();
}