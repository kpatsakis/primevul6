  bool parse_n(const char *s, size_t n, T &val,
               const char *path = nullptr) const {
    if (grammar_ != nullptr) {
      const auto &rule = (*grammar_)[start_];
      auto r = rule.parse_and_get_value(s, n, val, path);
      output_log(s, n, r);
      return r.ret && r.len == n;
    }
    return false;
  }