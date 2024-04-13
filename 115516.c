  Result parse(const char *s, size_t n, any &dt,
               const char *path = nullptr) const {
    SemanticValues sv;
    return parse_core(s, n, sv, dt, path);
  }