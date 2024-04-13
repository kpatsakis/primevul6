  size_t parse_core(const char *s, size_t n, SemanticValues &sv,
                    Context & /*c*/, any &dt) const override {
    assert(fn_);
    return fn_(s, n, sv, dt);
  }