static const char *php_mb_regex_mbctype2name(OnigEncoding mbctype) {
  php_mb_regex_enc_name_map_t *mapping;

  for (mapping = enc_name_map; mapping->names != nullptr; mapping++) {
    if (mapping->code == mbctype) {
      return mapping->names;
    }
  }

  return nullptr;
}