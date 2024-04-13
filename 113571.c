static Object _node_as_zval(SimpleXMLElement* sxe, xmlNodePtr node,
                            SXE_ITER itertype, const char* name,
                            const xmlChar* nsprefix, bool isprefix) {
  auto sxeObj = Native::object<SimpleXMLElement>(sxe);
  Object obj = create_object(sxeObj->getClassName(), Array(), false);
  auto subnode = Native::data<SimpleXMLElement>(obj.get());
  subnode->iter.type = itertype;
  if (name) {
    subnode->iter.name = xmlStrdup((xmlChar*)name);
  }
  if (nsprefix && *nsprefix) {
    subnode->iter.nsprefix = xmlStrdup(nsprefix);
    subnode->iter.isprefix = isprefix;
  }
  subnode->node = libxml_register_node(node);
  return obj;
}