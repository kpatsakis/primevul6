  SimpleXMLElement& operator=(const SimpleXMLElement &src) {
    iter.isprefix = src.iter.isprefix;
    if (src.iter.name != nullptr) {
      iter.name = xmlStrdup((xmlChar*)src.iter.name);
    }
    if (src.iter.nsprefix != nullptr) {
      iter.nsprefix = xmlStrdup((xmlChar*)src.iter.nsprefix);
    }
    iter.type = src.iter.type;

    if (src.nodep()) {
      node = libxml_register_node(
        xmlDocCopyNode(src.nodep(), src.docp(), 1)
      );
    }

    return *this;
  }