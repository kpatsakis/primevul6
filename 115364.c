  bool load_grammar(const char *s, const Rules &rules) {
    auto n = strlen(s);
    return load_grammar(s, n, rules);
  }