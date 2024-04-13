  size_t parse_core(const char *s, size_t n, SemanticValues &sv, Context &c,
                    any &dt) const override {
    if (c.in_whitespace) { return 0; }
    c.in_whitespace = true;
    auto se = make_scope_exit([&]() { c.in_whitespace = false; });
    const auto &rule = *ope_;
    return rule.parse(s, n, sv, c, dt);
  }