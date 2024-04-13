static void sxe_add_namespaces(SimpleXMLElement* sxe, xmlNodePtr node,
                               bool recursive, Array& return_value) {
  if (node->ns) {
    sxe_add_namespace_name(return_value, node->ns);
  }

  xmlAttrPtr attr = node->properties;
  while (attr) {
    if (attr->ns) {
      sxe_add_namespace_name(return_value, attr->ns);
    }
    attr = attr->next;
  }

  if (recursive) {
    node = node->children;
    while (node) {
      if (node->type == XML_ELEMENT_NODE) {
        sxe_add_namespaces(sxe, node, recursive, return_value);
      }
      node = node->next;
    }
  }
}