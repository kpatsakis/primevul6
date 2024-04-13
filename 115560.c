  size_t parse_core(const char *s, size_t n, SemanticValues &sv, Context &c,
                    any &dt) const override {
    size_t id = 0;
    for (const auto &ope : opes_) {
      auto &chldsv = c.push();
      c.push_capture_scope();
      auto se = make_scope_exit([&]() {
        c.pop();
        c.pop_capture_scope();
      });
      auto len = ope->parse(s, n, chldsv, c, dt);
      if (success(len)) {
        if (!chldsv.empty()) {
          for (size_t i = 0; i < chldsv.size(); i++) {
            sv.emplace_back(std::move(chldsv[i]));
          }
        }
        if (!chldsv.tags.empty()) {
          for (size_t i = 0; i < chldsv.tags.size(); i++) {
            sv.tags.emplace_back(std::move(chldsv.tags[i]));
          }
        }
        sv.s_ = chldsv.c_str();
        sv.n_ = chldsv.length();
        sv.choice_count_ = opes_.size();
        sv.choice_ = id;
        if (!chldsv.tokens.empty()) {
          for (size_t i = 0; i < chldsv.tokens.size(); i++) {
            sv.tokens.emplace_back(std::move(chldsv.tokens[i]));
          }
        }

        c.shift_capture_values();
        return len;
      }
      id++;
    }
    return static_cast<size_t>(-1);
  }