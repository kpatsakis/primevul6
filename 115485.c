inline size_t Dictionary::parse_core(const char *s, size_t n,
                                     SemanticValues & /*sv*/, Context &c,
                                     any & /*dt*/) const {
  auto len = trie_.match(s, n);
  if (len > 0) { return len; }
  c.set_error_pos(s);
  return static_cast<size_t>(-1);
}