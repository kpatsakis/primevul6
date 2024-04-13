  size_t match(const char *text, size_t text_len) const {
    size_t match_len = 0;
    {
      auto done = false;
      size_t len = 1;
      while (!done && len <= text_len) {
        std::string s(text, len);
        auto it = dic_.find(s);
        if (it == dic_.end()) {
          done = true;
        } else {
          if (it->second.match) { match_len = len; }
          if (it->second.done) { done = true; }
        }
        len += 1;
      }
    }
    return match_len;
  }