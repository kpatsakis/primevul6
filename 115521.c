  size_t parse_core(const char *s, size_t n, SemanticValues &sv, Context &c,
                    any &dt) const override {
    c.push_capture_scope();
    auto se = make_scope_exit([&]() { c.pop_capture_scope(); });
    const auto &rule = *ope_;
    auto len = rule.parse(s, n, sv, c, dt);
    return len;
  }