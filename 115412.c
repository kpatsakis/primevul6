  size_t parse_core(const char *s, size_t n, SemanticValues &sv, Context &c,
                    any &dt) const override {
    return parse_expression(s, n, sv, c, dt, 0);
  }