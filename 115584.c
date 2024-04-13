  size_t parse_core(const char *s, size_t n, SemanticValues & /*sv*/,
                    Context &c, any &dt) const override {
    auto save_error_pos = c.error_pos;
    auto &chldsv = c.push();
    c.push_capture_scope();
    auto se = make_scope_exit([&]() {
      c.pop();
      c.pop_capture_scope();
    });
    auto len = ope_->parse(s, n, chldsv, c, dt);
    if (success(len)) {
      c.set_error_pos(s);
      return static_cast<size_t>(-1);
    } else {
      c.error_pos = save_error_pos;
      return 0;
    }
  }