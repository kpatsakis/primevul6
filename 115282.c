  bool parse(const char *s, const char *path = nullptr) const {
    auto n = strlen(s);
    return parse_n(s, n, path);
  }