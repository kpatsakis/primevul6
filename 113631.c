static bool sxe_prop_dim_write(SimpleXMLElement* sxe, const Variant& member,
                               const Variant& value, bool elements, bool attribs,
                               xmlNodePtr* pnewnode) {
  xmlNodePtr node = sxe->nodep();

  if (member.isNull() || member.isInteger()) {
    if (sxe->iter.type != SXE_ITER_ATTRLIST) {
      attribs = false;
      elements = true;
    } else if (member.isNull()) {
      /* This happens when the user did: $sxe[] = $value
       * and could also be E_PARSE, but we use this only during parsing
       * and this is during runtime.
       */
      raise_error("Cannot create unnamed attribute");
      return false;
    }
  } else {
    if (member.toString().empty()) {
      raise_warning("Cannot write or create unnamed %s",
                    attribs ? "attribute" : "element");
      return false;
    }
  }

  bool retval = true;
  xmlAttrPtr attr   = nullptr;
  xmlNodePtr mynode = nullptr;
  bool test = false;
  if (sxe->iter.type == SXE_ITER_ATTRLIST) {
    attribs = true;
    elements = false;
    node = php_sxe_get_first_node(sxe, node);
    attr = (xmlAttrPtr)node;
    test = sxe->iter.name != nullptr;
  } else if (sxe->iter.type != SXE_ITER_CHILD) {
    mynode = node;
    node = php_sxe_get_first_node(sxe, node);
    attr = node ? node->properties : nullptr;
    test = false;
    if (member.isNull() && node && node->parent &&
        node->parent->type == XML_DOCUMENT_NODE) {
      /* This happens when the user did: $sxe[] = $value
       * and could also be E_PARSE, but we use this only during parsing
       * and this is during runtime.
       */
      raise_error("Cannot create unnamed attribute");
      return false;
    }
    if (attribs && !node && sxe->iter.type == SXE_ITER_ELEMENT) {
      node = xmlNewChild(mynode, mynode->ns, sxe->iter.name, nullptr);
      attr = node->properties;
    }
  }

  mynode = node;

  if (!(value.isString() || value.isInteger() || value.isBoolean() ||
      value.isDouble() || value.isNull() || value.isObject())) {
    raise_warning("It is not yet possible to assign complex types to %s",
                  attribs ? "attributes" : "properties");
    return false;
  }

  xmlNodePtr newnode = nullptr;
  if (node) {
    int64_t nodendx = 0;
    int64_t counter = 0;
    bool is_attr = false;
    if (attribs) {
      if (member.isInteger()) {
        while (attr && nodendx <= member.toInt64()) {
          if ((!test || !xmlStrcmp(attr->name, sxe->iter.name)) &&
              match_ns(sxe, (xmlNodePtr) attr, sxe->iter.nsprefix,
                       sxe->iter.isprefix)) {
            if (nodendx == member.toInt64()) {
              is_attr = true;
              ++counter;
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
            is_attr = true;
            ++counter;
            break;
          }
          attr = attr->next;
        }
      }

    }

    long cnt = 0;
    if (elements) {
      if (member.isNull() || member.isInteger()) {
        if (node->type == XML_ATTRIBUTE_NODE) {
          raise_error("Cannot create duplicate attribute");
          return false;
        }

        if (sxe->iter.type == SXE_ITER_NONE) {
          newnode = node;
          ++counter;
          if (!member.isNull() && member.toInt64() > 0) {
            raise_warning("Cannot add element %s number %" PRId64 " when "
                          "only 0 such elements exist", mynode->name,
                          member.toInt64());
            retval = false;
          }
        } else if (!member.isNull()) {
          newnode =
            sxe_get_element_by_offset(sxe, member.toInt64(), node, &cnt);
          if (newnode) {
            ++counter;
          }
        }
      } else {
        node = node->children;
        while (node) {
          SKIP_TEXT(node)

          if (!xmlStrcmp(node->name, (xmlChar*)member.toString().data())) {
            newnode = node;
            ++counter;
          }

next_iter:
          node = node->next;
        }
      }
    }

    if (counter == 1) {
      if (is_attr) {
        newnode = (xmlNodePtr) attr;
      }
      if (!value.isNull()) {
        xmlNodePtr tempnode;
        while ((tempnode = (xmlNodePtr) newnode->children)) {
          libxml_register_node(tempnode)->unlink();
        }
        change_node_zval(newnode, value);
      }
    } else if (counter > 1) {
      raise_warning("Cannot assign to an array of nodes "
                    "(duplicate subnodes or attr detected)");
      retval = false;
    } else if (elements) {
      if (!node) {
        if (member.isNull() || member.isInteger()) {
          newnode =
            xmlNewTextChild(
              mynode->parent, mynode->ns, mynode->name,
              !value.isNull() ? (xmlChar*)value.toString().data() : nullptr);
        } else {
          newnode =
            xmlNewTextChild(
              mynode, mynode->ns, (xmlChar*)member.toString().data(),
              !value.isNull() ? (xmlChar*)value.toString().data() : nullptr);
        }
      } else if (member.isNull() || member.isInteger()) {
        if (!member.isNull() && cnt < member.toInt64()) {
          raise_warning("Cannot add element %s number %" PRId64 " when "
                        "only %ld such elements exist", mynode->name,
                        member.toInt64(), cnt);
          retval = false;
        }
        newnode = xmlNewTextChild(mynode->parent, mynode->ns, mynode->name,
                                  !value.isNull() ?
                                  (xmlChar*)value.toString().data() : nullptr);
      }
    } else if (attribs) {
      if (member.isInteger()) {
        raise_warning("Cannot change attribute number %" PRId64 " when "
                      "only %" PRId64 " attributes exist", member.toInt64(),
                      nodendx);
        retval = false;
      } else {
        newnode = (xmlNodePtr)xmlNewProp(node,
                                         (xmlChar*)member.toString().data(),
                                         !value.isNull() ?
                                          (xmlChar*)value.toString().data() :
                                          nullptr);
      }
    }
  }

  if (pnewnode) {
    *pnewnode = newnode;
  }
  return retval;
}