  size_t parse_core(const char *s, size_t n, SemanticValues & /*sv*/,
                    Context &c, any &dt) const override {
    auto &chldsv = c.push();
    c.push_capture_scope();
    auto se = make_scope_exit([&]() {
      c.pop();
      c.pop_capture_scope();
    });
    const auto &rule = *ope_;
    auto len = rule.parse(s, n, chldsv, c, dt);
    if (success(len)) {
      return 0;
    } else {
      return static_cast<size_t>(-1);
    }
  }