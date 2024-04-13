inline size_t BackReference::parse_core(const char *s, size_t n,
                                        SemanticValues &sv, Context &c,
                                        any &dt) const {
  auto size = static_cast<int>(c.capture_scope_stack_size);
  for (auto i = size - 1; i >= 0; i--) {
    auto index = static_cast<size_t>(i);
    const auto &cs = c.capture_scope_stack[index];
    if (cs.find(name_) != cs.end()) {
      const auto &lit = cs.at(name_);
      std::once_flag init_is_word;
      auto is_word = false;
      return parse_literal(s, n, sv, c, dt, lit, init_is_word, is_word, false);
    }
  }
  throw std::runtime_error("Invalid back reference...");
}