  template <typename T = Ast> parser &enable_ast() {
    for (auto &x : *grammar_) {
      auto &rule = x.second;
      if (!rule.action) { add_ast_action<T>(rule); }
    }
    return *this;
  }