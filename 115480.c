  bool parse_n(const char *s, size_t n, any &dt,
               const char *path = nullptr) const {
    if (grammar_ != nullptr) {
      const auto &rule = (*grammar_)[start_];
      auto r = rule.parse(s, n, dt, path);
      output_log(s, n, r);
      return r.ret && r.len == n;
    }
    return false;
  }