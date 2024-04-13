static bool sxe_prop_dim_exists(SimpleXMLElement* sxe, const Variant& member,
                                bool check_empty, bool elements, bool attribs) {
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
  bool test = false;
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

  bool exists = false;
  if (node) {
    if (attribs) {
      if (member.isInteger()) {
        int64_t nodendx = 0;

        while (attr && nodendx <= member.toInt64()) {
          if ((!test || !xmlStrcmp(attr->name, sxe->iter.name)) &&
              match_ns(sxe, (xmlNodePtr) attr, sxe->iter.nsprefix,
                       sxe->iter.isprefix)) {
            if (nodendx == member.toInt64()) {
              exists = true;
              break;
            }
            nodendx++;
          }
          attr = attr->next;
        }
      } else {
        while (attr) {
          if ((!test || !xmlStrcmp(attr->name, sxe->iter.name)) &&
              !xmlStrcmp(attr->name, (xmlChar*)member.toString().data()) &&
              match_ns(sxe, (xmlNodePtr) attr, sxe->iter.nsprefix,
                       sxe->iter.isprefix)) {
            exists = true;
            break;
          }

          attr = attr->next;
        }
      }
      if (exists && check_empty == 1 &&
          (!attr->children || !attr->children->content ||
           !attr->children->content[0] ||
           !xmlStrcmp(attr->children->content, (const xmlChar*)"0")) ) {
        /* Attribute with no content in it's text node */
        exists = false;
      }
    }

    if (elements) {
      if (member.isInteger()) {
        if (sxe->iter.type == SXE_ITER_CHILD) {
          node = php_sxe_get_first_node(sxe, node);
        }
        node = sxe_get_element_by_offset(sxe, member.toInt64(), node, nullptr);
      }
      else {
        node = node->children;
        while (node) {
          xmlNodePtr nnext;
          nnext = node->next;
          if ((node->type == XML_ELEMENT_NODE) &&
              !xmlStrcmp(node->name, (xmlChar*)member.toString().data())) {
            break;
          }
          node = nnext;
        }
      }
      if (node) {
        exists = true;
        if (check_empty == true &&
            (!node->children || (node->children->type == XML_TEXT_NODE &&
                                 !node->children->next &&
                                 (!node->children->content ||
                                  !node->children->content[0] ||
                                  !xmlStrcmp(node->children->content,
                                             (const xmlChar*)"0"))))) {
          exists = false;
        }
      }
    }
  }

  return exists;
}