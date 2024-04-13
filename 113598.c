static xmlNodePtr php_sxe_iterator_fetch(SimpleXMLElement* sxe,
                                         xmlNodePtr node, int use_data) {
  xmlChar* prefix  = sxe->iter.nsprefix;
  bool isprefix  = sxe->iter.isprefix;
  bool test_elem = sxe->iter.type == SXE_ITER_ELEMENT  && sxe->iter.name;
  bool test_attr = sxe->iter.type == SXE_ITER_ATTRLIST && sxe->iter.name;

  while (node) {
    SKIP_TEXT(node)
    if (sxe->iter.type != SXE_ITER_ATTRLIST && node->type == XML_ELEMENT_NODE) {
      if ((!test_elem || !xmlStrcmp(node->name, sxe->iter.name))
          && match_ns(sxe, node, prefix, isprefix)) {
        break;
      }
    } else if (node->type == XML_ATTRIBUTE_NODE) {
      if ((!test_attr || !xmlStrcmp(node->name, sxe->iter.name)) &&
          match_ns(sxe, node, prefix, isprefix)) {
        break;
      }
    }
next_iter:
    node = node->next;
  }

  if (node && use_data) {
    sxe->iter.data = _node_as_zval(sxe, node, SXE_ITER_NONE, nullptr, prefix,
                                   isprefix);
  }

  return node;
}