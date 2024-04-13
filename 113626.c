static Variant HHVM_METHOD(SimpleXMLElement, xpath, const String& path) {
  auto data = Native::data<SimpleXMLElement>(this_);
  if (data->iter.type == SXE_ITER_ATTRLIST) {
    return init_null(); // attributes don't have attributes
  }

  if (!data->xpath) {
    data->xpath = xmlXPathNewContext(data->docp());
  }
  if (!data->nodep()) {
    data->node = libxml_register_node(xmlDocGetRootElement(data->docp()));
  }

  auto nodeptr = php_sxe_get_first_node(data, data->nodep());
  data->xpath->node = nodeptr;

  xmlNsPtr* ns = xmlGetNsList(data->docp(), nodeptr);
  int64_t nsnbr = 0;
  if (ns != nullptr) {
    while (ns[nsnbr] != nullptr) {
      nsnbr++;
    }
  }

  auto& xpath = data->xpath;
  xpath->namespaces = ns;
  xpath->nsNr = nsnbr;

  xmlXPathObjectPtr retval = xmlXPathEval((xmlChar*)path.data(), xpath);
  if (ns != nullptr) {
    xmlFree(ns);
    xpath->namespaces = nullptr;
    xpath->nsNr = 0;
  }

  if (!retval) {
    return false;
  }

  xmlNodeSetPtr result = retval->nodesetval;

  Array ret = Array::Create();
  if (result != nullptr) {
    for (int64_t i = 0; i < result->nodeNr; ++i) {
      nodeptr = result->nodeTab[i];
      if (nodeptr->type == XML_TEXT_NODE ||
          nodeptr->type == XML_ELEMENT_NODE ||
          nodeptr->type == XML_ATTRIBUTE_NODE) {
        /**
         * Detect the case where the last selector is text(), simplexml
         * always accesses the text() child by default, therefore we assign
         * to the parent node.
         */
        Object obj;
        if (nodeptr->type == XML_TEXT_NODE) {
          obj = _node_as_zval(data, nodeptr->parent, SXE_ITER_NONE, nullptr,
                              nullptr, false);
        } else if (nodeptr->type == XML_ATTRIBUTE_NODE) {
          obj = _node_as_zval(data, nodeptr->parent, SXE_ITER_ATTRLIST,
                              (char*)nodeptr->name, nodeptr->ns ?
                                (xmlChar*)nodeptr->ns->href : nullptr, false);
        } else {
          obj = _node_as_zval(data, nodeptr, SXE_ITER_NONE, nullptr, nullptr,
                              false);
        }
        if (!obj.isNull()) {
          ret.append(obj);
        }
      }
    }
  }

  xmlXPathFreeObject(retval);
  return ret;
}