static Object sxe_prop_dim_read(SimpleXMLElement* sxe, const Variant& member,
                                bool elements, bool attribs) {
  xmlNodePtr node = sxe->nodep();

  String name = "";
  if (member.isNull() || member.isInteger()) {
    if (sxe->iter.type != SXE_ITER_ATTRLIST) {
      attribs = false;
      elements = true;
    } else if (member.isNull()) {
      /* This happens when the user did: $sxe[]->foo = $value */
      raise_error("Cannot create unnamed attribute");
      return Object{};
    }
  } else {
    name = member.toString();
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
    if (member.isNull() && node && node->parent &&
        node->parent->type == XML_DOCUMENT_NODE) {
      /* This happens when the user did: $sxe[]->foo = $value */
      raise_error("Cannot create unnamed attribute");
      return Object{};
    }
  }

  Object return_value;
  if (node) {
    if (attribs) {
      if (!member.isInteger() || sxe->iter.type == SXE_ITER_ATTRLIST) {
        if (member.isInteger()) {
          int64_t nodendx = 0;
          while (attr && nodendx <= member.toInt64()) {
            if ((!test || !xmlStrcmp(attr->name, sxe->iter.name)) &&
                match_ns(sxe, (xmlNodePtr) attr, sxe->iter.nsprefix,
                         sxe->iter.isprefix)) {
              if (nodendx == member.toInt64()) {
                return_value = _node_as_zval(sxe, (xmlNodePtr) attr,
                                             SXE_ITER_NONE, nullptr,
                                             sxe->iter.nsprefix,
                                             sxe->iter.isprefix);
                break;
              }
              nodendx++;
            }
            attr = attr->next;
          }
        } else {
          while (attr) {
            if ((!test || !xmlStrcmp(attr->name, sxe->iter.name)) &&
                !xmlStrcmp(attr->name, (xmlChar*)name.data()) &&
                match_ns(sxe, (xmlNodePtr) attr, sxe->iter.nsprefix,
                         sxe->iter.isprefix)) {
              return_value = _node_as_zval(sxe, (xmlNodePtr) attr,
                                           SXE_ITER_NONE,
                                           nullptr,
                                           sxe->iter.nsprefix,
                                           sxe->iter.isprefix);
              break;
            }
            attr = attr->next;
          }
        }
      }
    }

    if (elements) {
      if (!sxe->nodep()) {
        sxe->node = libxml_register_node(node);
      }
      if (member.isNull() || member.isInteger()) {
        long cnt = 0;
        xmlNodePtr mynode = node;

        if (sxe->iter.type == SXE_ITER_CHILD) {
          node = php_sxe_get_first_node(sxe, node);
        }
        if (sxe->iter.type == SXE_ITER_NONE) {
          if (!member.isNull() && member.toInt64() > 0) {
            raise_warning("Cannot add element %s number %" PRId64 " when "
                          "only 0 such elements exist", mynode->name,
                          member.toInt64());
          }
        } else if (!member.isNull()) {
          node = sxe_get_element_by_offset(sxe, member.toInt64(), node, &cnt);
        } else {
          node = nullptr;
        }
        if (node) {
          return_value = _node_as_zval(sxe, node, SXE_ITER_NONE, nullptr,
                                       sxe->iter.nsprefix, sxe->iter.isprefix);
        }
        // Zend would check here if this is a write operation, but HHVM always
        // handles that with offsetSet so we just want to return nullptr here.
      } else {
#if SXE_ELEMENT_BY_NAME
        int newtype;

        node = sxe->nodep();
        node = sxe_get_element_by_name(sxe, node, &name.data(), &newtype);
        if (node) {
          return_value = _node_as_zval(sxe, node, newtype, name.data(),
                                       sxe->iter.nsprefix, sxe->iter.isprefix);
        }
#else
        return_value = _node_as_zval(sxe, node, SXE_ITER_ELEMENT, name.data(),
                                     sxe->iter.nsprefix, sxe->iter.isprefix);
#endif
      }
    }
  }

  return return_value;
}