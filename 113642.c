static Variant HHVM_FUNCTION(simplexml_import_dom,
                             const Object& node,
                             const String& class_name) {
  if (!node->instanceof(s_DOMNode)) {
    raise_warning("Invalid Nodetype to import");
    return init_null();
  }
  auto domnode = Native::data<DOMNode>(node);
  xmlNodePtr nodep = domnode->nodep();

  if (nodep) {
    if (nodep->doc == nullptr) {
      raise_warning("Imported Node must have associated Document");
      return init_null();
    }
    if (nodep->type == XML_DOCUMENT_NODE ||
        nodep->type == XML_HTML_DOCUMENT_NODE) {
      nodep = xmlDocGetRootElement((xmlDocPtr) nodep);
    }
  }

  if (nodep && nodep->type == XML_ELEMENT_NODE) {
    auto cls = class_from_name(class_name, "simplexml_import_dom");
    if (!cls) {
      return init_null();
    }
    Object obj = create_object(cls->nameStr(), Array(), false);
    auto sxe = Native::data<SimpleXMLElement>(obj.get());
    sxe->node = libxml_register_node(nodep);
    return obj;
  } else {
    raise_warning("Invalid Nodetype to import");
    return init_null();
  }
  return false;
}