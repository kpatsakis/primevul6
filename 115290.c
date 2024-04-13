  Result parse_and_get_value(const char *s, T &val,
                             const char *path = nullptr) const {
    auto n = strlen(s);
    return parse_and_get_value(s, n, val, path);
  }