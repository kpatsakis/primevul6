inline size_t TokenBoundary::parse_core(const char *s, size_t n,
                                        SemanticValues &sv, Context &c,
                                        any &dt) const {
  c.in_token = true;
  auto se = make_scope_exit([&]() { c.in_token = false; });
  auto len = ope_->parse(s, n, sv, c, dt);
  if (success(len)) {
    sv.tokens.emplace_back(std::make_pair(s, len));

    if (c.whitespaceOpe) {
      auto l = c.whitespaceOpe->parse(s + len, n - len, sv, c, dt);
      if (fail(l)) { return static_cast<size_t>(-1); }
      len += l;
    }
  }
  return len;
}