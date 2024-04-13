  void requestInit() override {
    current_language = language;
    current_internal_encoding = internal_encoding;
    current_http_output_encoding = http_output_encoding;
    current_filter_illegal_mode = filter_illegal_mode;
    current_filter_illegal_substchar = filter_illegal_substchar;
    if (!encoding_translation) {
      illegalchars = 0;
    }

    mbfl_encoding **entry = nullptr;
    int n = 0;
    if (current_detect_order_list) {
      return;
    }

    if (detect_order_list && detect_order_list_size > 0) {
      n = detect_order_list_size;
      entry = (mbfl_encoding **)req::malloc_noptrs(n * sizeof(mbfl_encoding*));
      std::copy(detect_order_list,
                detect_order_list + (n * sizeof(mbfl_encoding*)), entry);
    } else {
      mbfl_no_encoding *src = default_detect_order_list;
      n = default_detect_order_list_size;
      entry = (mbfl_encoding **)req::malloc_noptrs(n * sizeof(mbfl_encoding*));
      for (int i = 0; i < n; i++) {
        entry[i] = (mbfl_encoding*) mbfl_no2encoding(src[i]);
      }
    }

    current_detect_order_list = entry;
    current_detect_order_list_size = n;
  }