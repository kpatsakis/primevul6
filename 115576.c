  bool parse(const char *s, T &val, const char *path = nullptr) const {
    auto n = strlen(s);
    return parse_n(s, n, val, path);
  }