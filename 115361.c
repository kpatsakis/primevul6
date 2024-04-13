  size_t parse_core(const char *s, size_t n, SemanticValues & /*sv*/,
                    Context &c, any &dt) const override {
    const auto &rule = *ope_;
    auto &chldsv = c.push();
    auto se = make_scope_exit([&]() { c.pop(); });
    return rule.parse(s, n, chldsv, c, dt);
  }