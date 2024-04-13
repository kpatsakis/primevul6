  size_t parse_core(const char *s, size_t n, SemanticValues &sv, Context &c,
                    any &dt) const override {
    const auto &rule = *ope_;
    auto len = rule.parse(s, n, sv, c, dt);
    if (success(len) && match_action_) { match_action_(s, len, c); }
    return len;
  }