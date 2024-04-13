static void php_sxe_move_forward_iterator(SimpleXMLElement* sxe) {
  xmlNodePtr node = nullptr;
  auto data = sxe->iter.data;
  if (!data.isNull()) {
    assert(data->instanceof(SimpleXMLElement_classof()));
    auto intern = Native::data<SimpleXMLElement>(data.get());
    node = intern->nodep();
    sxe->iter.data.reset();
  }

  if (node) {
    php_sxe_iterator_fetch(sxe, node->next, 1);
  }
}