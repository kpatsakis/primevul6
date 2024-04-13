  size_t parse_core(const char *s, size_t n, SemanticValues &sv, Context &c,
                    any &dt) const override {
    auto &chldsv = c.push();
    auto pop_se = make_scope_exit([&]() { c.pop(); });
    size_t i = 0;
    for (const auto &ope : opes_) {
      const auto &rule = *ope;
      auto len = rule.parse(s + i, n - i, chldsv, c, dt);
      if (fail(len)) { return static_cast<size_t>(-1); }
      i += len;
    }
    if (!chldsv.empty()) {
      for (size_t j = 0; j < chldsv.size(); j++) {
        sv.emplace_back(std::move(chldsv[j]));
      }
    }
    if (!chldsv.tags.empty()) {
      for (size_t j = 0; j < chldsv.tags.size(); j++) {
        sv.tags.emplace_back(std::move(chldsv.tags[j]));
      }
    }
    sv.s_ = chldsv.c_str();
    sv.n_ = chldsv.length();
    if (!chldsv.tokens.empty()) {
      for (size_t j = 0; j < chldsv.tokens.size(); j++) {
        sv.tokens.emplace_back(std::move(chldsv.tokens[j]));
      }
    }
    return i;
  }