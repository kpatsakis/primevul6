  void sweep() {
    if (iter.name) {
      xmlFree(iter.name);
    }
    if (iter.nsprefix) {
      xmlFree(iter.nsprefix);
    }
    if (xpath) {
      xmlXPathFreeContext(xpath);
    }
  }