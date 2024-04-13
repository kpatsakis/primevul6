static void sxe_prop_dim_delete(SimpleXMLElement* sxe, const Variant& member,
                                bool elements, bool attribs) {
  xmlNodePtr node = sxe->nodep();

  if (member.isInteger()) {
    if (sxe->iter.type != SXE_ITER_ATTRLIST) {
      attribs = false;
      elements = true;
      if (sxe->iter.type == SXE_ITER_CHILD) {
        node = php_sxe_get_first_node(sxe, node);
      }
    }
  }

  xmlAttrPtr attr = nullptr;
  bool test = 0;
  if (sxe->iter.type == SXE_ITER_ATTRLIST) {
    attribs = true;
    elements = false;
    node = php_sxe_get_first_node(sxe, node);
    attr = (xmlAttrPtr)node;
    test = sxe->iter.name != nullptr;
  } else if (sxe->iter.type != SXE_ITER_CHILD) {
    node = php_sxe_get_first_node(sxe, node);
    attr = node ? node->properties : nullptr;
    test = false;
  }

  if (node) {
    if (attribs) {
      if (member.isInteger()) {
        int64_t nodendx = 0;

        while (attr && nodendx <= member.toInt64()) {
          if ((!test || !xmlStrcmp(attr->name, sxe->iter.name)) &&
              match_ns(sxe, (xmlNodePtr) attr, sxe->iter.nsprefix,
                       sxe->iter.isprefix)) {
            if (nodendx == member.toInt64()) {
              libxml_register_node((xmlNodePtr) attr)->unlink();
              break;
            }
            nodendx++;
          }
          attr = attr->next;
        }
      } else {
        xmlAttrPtr anext;
        while (attr) {
          anext = attr->next;
          if ((!test || !xmlStrcmp(attr->name, sxe->iter.name)) &&
              !xmlStrcmp(attr->name, (xmlChar*)member.toString().data()) &&
              match_ns(sxe, (xmlNodePtr) attr, sxe->iter.nsprefix,
                       sxe->iter.isprefix)) {
            libxml_register_node((xmlNodePtr) attr)->unlink();
            break;
          }
          attr = anext;
        }
      }
    }

    if (elements) {
      if (member.isInteger()) {
        if (sxe->iter.type == SXE_ITER_CHILD) {
          node = php_sxe_get_first_node(sxe, node);
        }
        node = sxe_get_element_by_offset(sxe, member.toInt64(), node, nullptr);
        if (node) {
          libxml_register_node(node)->unlink();
        }
      } else {
        node = node->children;
        xmlNodePtr nnext;
        while (node) {
          nnext = node->next;

          SKIP_TEXT(node);

          if (!xmlStrcmp(node->name, (xmlChar*)member.toString().data())) {
            libxml_register_node(node)->unlink();
          }

next_iter:
          node = nnext;
        }
      }
    }
  }
}