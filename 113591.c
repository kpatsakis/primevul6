static void change_node_zval(xmlNodePtr node, const Variant& value) {
  if (value.isNull()) {
    xmlNodeSetContentLen(node, (xmlChar*)"", 0);
    return;
  }
  if (value.isInteger() || value.isBoolean() || value.isDouble() ||
      value.isNull() || value.isString()) {
      xmlChar* buffer =
        xmlEncodeEntitiesReentrant(node->doc,
                                   (xmlChar*)value.toString().data());
      int64_t buffer_len = xmlStrlen(buffer);
      /* check for nullptr buffer in case of
       * memory error in xmlEncodeEntitiesReentrant */
      if (buffer) {
        xmlNodeSetContentLen(node, buffer, buffer_len);
        xmlFree(buffer);
      }
  } else {
    raise_warning("It is not possible to assign complex types to nodes");
  }
}