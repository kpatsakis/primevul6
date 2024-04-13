  size_t parse_core(const char *s, size_t n, SemanticValues & /*sv*/,
                    Context &c, any & /*dt*/) const override {
    if (n < 1 || s[0] != ch_) {
      c.set_error_pos(s);
      return static_cast<size_t>(-1);
    }
    return 1;
  }