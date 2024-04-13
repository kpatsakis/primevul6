static inline bool match_ns(SimpleXMLElement* sxe, xmlNodePtr node,
                            xmlChar* name, bool prefix) {
  if (name == nullptr && (node->ns == nullptr || node->ns->prefix == nullptr)) {
    return true;
  }

  if (RuntimeOption::SimpleXMLEmptyNamespaceMatchesAll &&
      (name == nullptr || *name == '\0')) {
    return true;
  }

  if (node->ns &&
      !xmlStrcmp(prefix ? node->ns->prefix : node->ns->href, name)) {
    return true;
  }

  return false;
}