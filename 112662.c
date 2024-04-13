Variant HHVM_FUNCTION(mb_detect_encoding,
                      const String& str,
                      const Variant& encoding_list /* = uninit_variant */,
                      const Variant& strict /* = uninit_variant */) {
  mbfl_string string;
  mbfl_encoding *ret;
  mbfl_encoding **elist, **list;
  int size;

  /* make encoding list */
  list = nullptr;
  size = 0;
  php_mb_parse_encoding(encoding_list, &list, &size, false);
  if (size > 0 && list != nullptr) {
    elist = list;
  } else {
    elist = MBSTRG(current_detect_order_list);
    size = MBSTRG(current_detect_order_list_size);
  }

  long nstrict = 0;
  if (!strict.isNull()) {
    nstrict = strict.toInt64();
  } else {
    nstrict = MBSTRG(strict_detection);
  }

  mbfl_string_init(&string);
  string.no_language = MBSTRG(current_language);
  string.val = (unsigned char *)str.data();
  string.len = str.size();
  ret = (mbfl_encoding*) mbfl_identify_encoding2(&string,
                                                 (const mbfl_encoding**) elist,
                                                 size, nstrict);
  req::free(list);
  if (ret != nullptr) {
    return String(ret->name, CopyString);
  }
  return false;
}