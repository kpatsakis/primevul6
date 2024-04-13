inline size_t LiteralString::parse_core(const char *s, size_t n,
                                        SemanticValues &sv, Context &c,
                                        any &dt) const {
  return parse_literal(s, n, sv, c, dt, lit_, init_is_word_, is_word_,
                       ignore_case_);
}