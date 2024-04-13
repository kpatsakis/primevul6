php_mb_parse_encoding_array(const Array& array, mbfl_encoding*** return_list,
                            int* return_size, int /*persistent*/) {
  int n, l, size, bauto,ret = 1;
  mbfl_encoding *encoding;
  mbfl_no_encoding *src;
  mbfl_encoding **list, **entry;

  list = nullptr;
  mbfl_no_encoding *identify_list = MBSTRG(default_detect_order_list);
  int identify_list_size = MBSTRG(default_detect_order_list_size);

  size = array.size() + identify_list_size;
  list = (mbfl_encoding **)req::calloc_noptrs(size, sizeof(mbfl_encoding*));
  if (list != nullptr) {
    entry = list;
    bauto = 0;
    n = 0;
    for (ArrayIter iter(array); iter; ++iter) {
      auto const hash_entry = iter.second().toString();
      if (strcasecmp(hash_entry.data(), "auto") == 0) {
        if (!bauto) {
          bauto = 1;
          l = identify_list_size;
          src = identify_list;
          for (int j = 0; j < l; j++) {
            *entry++ = (mbfl_encoding*) mbfl_no2encoding(*src++);
            n++;
          }
        }
      } else {
        encoding = (mbfl_encoding*) mbfl_name2encoding(hash_entry.data());
        if (encoding != nullptr) {
          *entry++ = encoding;
          n++;
        } else {
          ret = 0;
        }
      }
    }
    if (n > 0) {
      if (return_list) {
        *return_list = list;
      } else {
        req::free(list);
      }
    } else {
      req::free(list);
      if (return_list) {
        *return_list = nullptr;
      }
      ret = 0;
    }
    if (return_size) {
      *return_size = n;
    }
  } else {
    if (return_list) {
      *return_list = nullptr;
    }
    if (return_size) {
      *return_size = 0;
    }
    ret = 0;
  }
  return ret;
}