static void sxe_add_registered_namespaces(SimpleXMLElement* sxe,
                                          xmlNodePtr node, bool recursive,
                                          Array& return_value) {
  if (node != nullptr && node->type == XML_ELEMENT_NODE) {
    xmlNsPtr ns = node->nsDef;
    while (ns != nullptr) {
      sxe_add_namespace_name(return_value, ns);
      ns = ns->next;
    }
    if (recursive) {
      node = node->children;
      while (node) {
        sxe_add_registered_namespaces(sxe, node, recursive, return_value);
        node = node->next;
      }
    }
  }
}