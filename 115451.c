  Result parse_and_get_value(const char *s, size_t n, T &val,
                             const char *path = nullptr) const {
    SemanticValues sv;
    any dt;
    auto r = parse_core(s, n, sv, dt, path);
    if (r.ret && !sv.empty() && sv.front().has_value()) {
      val = any_cast<T>(sv[0]);
    }
    return r;
  }