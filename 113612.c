static int64_t php_sxe_count_elements_helper(SimpleXMLElement* sxe) {
  Object data = sxe->iter.data;
  sxe->iter.data.reset();

  xmlNodePtr node = php_sxe_reset_iterator(sxe, false);
  int64_t count = 0;
  while (node) {
    count++;
    node = php_sxe_iterator_fetch(sxe, node->next, 0);
  }

  sxe->iter.data = data;
  return count;
}