static char *php_mime_get_hdr_value(header_list &header, char *key) {
  if (key == nullptr) return nullptr;
  for (header_list::iterator iter = header.begin();
       iter != header.end(); iter++) {
    if (!strcasecmp(iter->first.c_str(), key)) {
      return (char *)iter->second.c_str();
    }
  }
  return nullptr;
}