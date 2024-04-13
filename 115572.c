  size_t parse_core(const char *s, size_t n, SemanticValues &sv, Context &c,
                    any &dt) const override {
    auto ope = weak_.lock();
    assert(ope);
    const auto &rule = *ope;
    return rule.parse(s, n, sv, c, dt);
  }