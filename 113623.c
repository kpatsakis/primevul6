static xmlNodePtr php_sxe_reset_iterator(SimpleXMLElement* sxe,
                                         bool use_data) {
  if (!sxe->iter.data.isNull()) {
    sxe->iter.data.reset();
  }

  xmlNodePtr node = sxe->nodep();
  if (node) {
    switch (sxe->iter.type) {
      case SXE_ITER_ELEMENT:
      case SXE_ITER_CHILD:
      case SXE_ITER_NONE:
        node = node->children;
        break;
      case SXE_ITER_ATTRLIST:
        node = (xmlNodePtr)node->properties;
    }
    return php_sxe_iterator_fetch(sxe, node, use_data);
  }
  return nullptr;
}