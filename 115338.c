  Result parse(const char *s, const char *path = nullptr) const {
    auto n = strlen(s);
    return parse(s, n, path);
  }