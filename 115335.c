  size_t parse_core(const char *s, size_t n, SemanticValues & /*sv*/,
                    Context &c, any & /*dt*/) const override {
    auto len = codepoint_length(s, n);
    if (len < 1) {
      c.set_error_pos(s);
      return static_cast<size_t>(-1);
    }
    return len;
  }