static inline String sxe_xmlNodeListGetString(xmlDocPtr doc, xmlNodePtr list,
                                              bool inLine) {
  xmlChar* tmp = xmlNodeListGetString(doc, list, inLine);
  if (tmp) {
    String ret = String((char*)tmp);
    xmlFree(tmp);
    return ret;
  } else {
    return empty_string();
  }
}