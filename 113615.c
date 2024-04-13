static xmlNodePtr sxe_get_element_by_offset(SimpleXMLElement* sxe,
                                            long offset, xmlNodePtr node,
                                            long* cnt) {
  if (sxe->iter.type == SXE_ITER_NONE) {
    if (offset == 0) {
      if (cnt) {
        *cnt = 0;
      }
      return node;
    } else {
      return nullptr;
    }
  }

  long nodendx = 0;
  while (node && nodendx <= offset) {
    SKIP_TEXT(node)
    if (node->type == XML_ELEMENT_NODE &&
        match_ns(sxe, node, sxe->iter.nsprefix, sxe->iter.isprefix)) {
      if (sxe->iter.type == SXE_ITER_CHILD ||
          (sxe->iter.type == SXE_ITER_ELEMENT
           && !xmlStrcmp(node->name, sxe->iter.name))) {
        if (nodendx == offset) {
          break;
        }
        nodendx++;
      }
    }
next_iter:
    node = node->next;
  }

  if (cnt) {
    *cnt = nodendx;
  }

  return node;
}