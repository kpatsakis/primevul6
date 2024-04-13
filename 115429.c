  bool parse(const char *s, any &dt, T &val,
             const char * /*path*/ = nullptr) const {
    auto n = strlen(s);
    return parse_n(s, n, dt, val);
  }