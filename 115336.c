  bool parse_n(const char *s, size_t n, const char *path = nullptr) const {
    if (grammar_ != nullptr) {
      const auto &rule = (*grammar_)[start_];
      auto r = rule.parse(s, n, path);
      output_log(s, n, r);
      return r.ret && r.len == n;
    }
    return false;
  }