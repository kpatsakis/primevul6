inline size_t parse_literal(const char *s, size_t n, SemanticValues &sv,
                            Context &c, any &dt, const std::string &lit,
                            std::once_flag &init_is_word, bool &is_word,
                            bool ignore_case) {
  size_t i = 0;
  for (; i < lit.size(); i++) {
    if (i >= n || (ignore_case ? (std::tolower(s[i]) != std::tolower(lit[i]))
                               : (s[i] != lit[i]))) {
      c.set_error_pos(s);
      return static_cast<size_t>(-1);
    }
  }

  // Word check
  static Context dummy_c(nullptr, c.s, c.l, 0, nullptr, nullptr, false, nullptr,
                         nullptr);
  static SemanticValues dummy_sv;
  static any dummy_dt;

  std::call_once(init_is_word, [&]() {
    if (c.wordOpe) {
      auto len =
          c.wordOpe->parse(lit.data(), lit.size(), dummy_sv, dummy_c, dummy_dt);
      is_word = success(len);
    }
  });

  if (is_word) {
    NotPredicate ope(c.wordOpe);
    auto len = ope.parse(s + i, n - i, dummy_sv, dummy_c, dummy_dt);
    if (fail(len)) { return static_cast<size_t>(-1); }
    i += len;
  }

  // Skip whiltespace
  if (!c.in_token) {
    if (c.whitespaceOpe) {
      auto len = c.whitespaceOpe->parse(s + i, n - i, sv, c, dt);
      if (fail(len)) { return static_cast<size_t>(-1); }
      i += len;
    }
  }

  return i;
}