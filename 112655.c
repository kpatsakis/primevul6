  void requestShutdown() override {
    if (current_detect_order_list != nullptr) {
      req::free(current_detect_order_list);
      current_detect_order_list = nullptr;
      current_detect_order_list_size = 0;
    }
    if (outconv != nullptr) {
      illegalchars += mbfl_buffer_illegalchars(outconv);
      mbfl_buffer_converter_delete(outconv);
      outconv = nullptr;
    }

    /* clear http input identification. */
    http_input_identify = nullptr;
    http_input_identify_post = nullptr;
    http_input_identify_get = nullptr;
    http_input_identify_cookie = nullptr;
    http_input_identify_string = nullptr;

    current_mbctype = default_mbctype;

    search_str.clear();
    search_pos = 0;

    if (search_regs != nullptr) {
      onig_region_free(search_regs, 1);
      search_regs = (OnigRegion *)nullptr;
    }
    for (RegexCache::const_iterator it = ht_rc.begin(); it != ht_rc.end();
         ++it) {
      onig_free(it->second);
    }
    ht_rc.clear();
  }