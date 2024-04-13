  Result parse(const char *s, size_t n, const char *path = nullptr) const {
    SemanticValues sv;
    any dt;
    return parse_core(s, n, sv, dt, path);
  }