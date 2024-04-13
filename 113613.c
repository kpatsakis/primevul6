static void sxe_get_prop_hash(SimpleXMLElement* sxe, bool is_debug,
                              Array& rv, bool isBoolCast = false) {
  rv.clear();

  Object iter_data;
  bool use_iter = false;
  xmlNodePtr node = sxe->nodep();
  if (!node) {
    return;
  }
  if (is_debug || sxe->iter.type != SXE_ITER_CHILD) {
    if (sxe->iter.type == SXE_ITER_ELEMENT) {
      node = php_sxe_get_first_node(sxe, node);
    }
    if (!node || node->type != XML_ENTITY_DECL) {
      xmlAttrPtr attr = node ? (xmlAttrPtr)node->properties : nullptr;
      Array zattr = Array::Create();
      bool test = sxe->iter.name && sxe->iter.type == SXE_ITER_ATTRLIST;
      while (attr) {
        if ((!test || !xmlStrcmp(attr->name, sxe->iter.name)) &&
            match_ns(sxe, (xmlNodePtr)attr, sxe->iter.nsprefix,
                     sxe->iter.isprefix)) {
          zattr.set(String((char*)attr->name),
                    sxe_xmlNodeListGetString(
                      sxe->docp(),
                      attr->children,
                      1));
        }
        attr = attr->next;
      }
      if (zattr.size()) {
        rv.set(String("@attributes"), zattr);
      }
    }
  }

  node = sxe->nodep();
  node = php_sxe_get_first_node(sxe, node);

  if (node && sxe->iter.type != SXE_ITER_ATTRLIST) {
    if (node->type == XML_ATTRIBUTE_NODE) {
      rv.append(sxe_xmlNodeListGetString(node->doc, node->children, 1));
      node = nullptr;
    } else if (sxe->iter.type != SXE_ITER_CHILD) {
      if (sxe->iter.type == SXE_ITER_NONE || !node->children ||
          !node->parent ||
          node->children->next ||
          node->children->children ||
          node->parent->children == node->parent->last) {
        node = node->children;
      } else {
        iter_data = sxe->iter.data;
        sxe->iter.data.reset();

        node = php_sxe_reset_iterator(sxe, false);

        use_iter = true;
      }
    }

    char *name  = nullptr;
    Variant value;
    while (node) {
      if (node->children != nullptr || node->prev != nullptr ||
          node->next != nullptr) {
        SKIP_TEXT(node);
      } else {
        if (node->type == XML_TEXT_NODE) {
          const xmlChar* cur = node->content;

          if (*cur != 0) {
            rv.append(sxe_xmlNodeListGetString(node->doc, node, 1));
          }
          goto next_iter;
        }
      }

      if (node->type == XML_ELEMENT_NODE &&
          (!match_ns(sxe, node, sxe->iter.nsprefix, sxe->iter.isprefix))) {
        goto next_iter;
      }

      name = (char*)node->name;
      if (!name) {
        goto next_iter;
      }

      value = _get_base_node_value(sxe, node, sxe->iter.nsprefix,
                                   sxe->iter.isprefix);
      if (use_iter) {
        rv.append(value);
      } else {
        sxe_properties_add(rv, name, value);
      }
      if (isBoolCast) break;
next_iter:
      if (use_iter) {
        node = php_sxe_iterator_fetch(sxe, node->next, 0);
      } else {
        node = node->next;
      }
    }
  }

  if (use_iter) {
    sxe->iter.data = iter_data;
  }
}