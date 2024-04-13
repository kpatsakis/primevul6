  size_t parse_core(const char *s, size_t n, SemanticValues &sv, Context &c,
                    any &dt) const override {
    size_t count = 0;
    size_t i = 0;
    while (count < min_) {
      c.push_capture_scope();
      auto se = make_scope_exit([&]() { c.pop_capture_scope(); });
      const auto &rule = *ope_;
      auto len = rule.parse(s + i, n - i, sv, c, dt);
      if (success(len)) {
        c.shift_capture_values();
      } else {
        return static_cast<size_t>(-1);
      }
      i += len;
      count++;
    }

    auto save_error_pos = c.error_pos;
    while (n - i > 0 && count < max_) {
      c.push_capture_scope();
      auto se = make_scope_exit([&]() { c.pop_capture_scope(); });
      auto save_sv_size = sv.size();
      auto save_tok_size = sv.tokens.size();
      const auto &rule = *ope_;
      auto len = rule.parse(s + i, n - i, sv, c, dt);
      if (success(len)) {
        c.shift_capture_values();
      } else {
        if (sv.size() != save_sv_size) {
          sv.erase(sv.begin() + static_cast<std::ptrdiff_t>(save_sv_size));
          sv.tags.erase(sv.tags.begin() +
                        static_cast<std::ptrdiff_t>(save_sv_size));
        }
        if (sv.tokens.size() != save_tok_size) {
          sv.tokens.erase(sv.tokens.begin() +
                          static_cast<std::ptrdiff_t>(save_tok_size));
        }
        c.error_pos = save_error_pos;
        break;
      }
      i += len;
      count++;
    }
    return i;
  }