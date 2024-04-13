  Result parse(const char *s, any &dt, const char *path = nullptr) const {
    auto n = strlen(s);
    return parse(s, n, dt, path);
  }