  bool parse(const char *s, any &dt, const char *path = nullptr) const {
    auto n = strlen(s);
    return parse_n(s, n, dt, path);
  }