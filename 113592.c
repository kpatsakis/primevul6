static xmlNodePtr php_sxe_get_first_node(SimpleXMLElement* sxe,
                                         xmlNodePtr node) {
  if (sxe && sxe->iter.type != SXE_ITER_NONE) {
    php_sxe_reset_iterator(sxe, true);
    xmlNodePtr retnode = nullptr;
    if (!sxe->iter.data.isNull()) {
      assert(sxe->iter.data->instanceof(SimpleXMLElement_classof()));
      retnode = Native::data<SimpleXMLElement>(sxe->iter.data.get())->nodep();
    }
    return retnode;
  } else {
    return node;
  }
}