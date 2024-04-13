Variant HHVM_FUNCTION(mb_detect_order,
                      const Variant& encoding_list /* = uninit_variant */) {
  int n, size;
  mbfl_encoding **list, **entry;

  if (encoding_list.isNull()) {
    Array ret;
    entry = MBSTRG(current_detect_order_list);
    n = MBSTRG(current_detect_order_list_size);
    while (n > 0) {
      char *name = (char*) (*entry)->name;
      if (name) {
        ret.append(String(name, CopyString));
      }
      entry++;
      n--;
    }
    return ret;
  }

  list = nullptr;
  size = 0;
  if (!php_mb_parse_encoding(encoding_list, &list, &size, false) ||
      list == nullptr) {
    return false;
  }
  if (MBSTRG(current_detect_order_list)) {
    req::free(MBSTRG(current_detect_order_list));
  }
  MBSTRG(current_detect_order_list) = list;
  MBSTRG(current_detect_order_list_size) = size;
  return true;
}