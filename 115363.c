template <typename T = Ast> void add_ast_action(Definition &rule) {
  rule.action = [&](const SemanticValues &sv) {
    auto line = sv.line_info();

    if (rule.is_token()) {
      return std::make_shared<T>(sv.path, line.first, line.second,
                                 rule.name.c_str(), sv.token(),
                                 std::distance(sv.ss, sv.c_str()), sv.length(),
                                 sv.choice_count(), sv.choice());
    }

    auto ast = std::make_shared<T>(
        sv.path, line.first, line.second, rule.name.c_str(),
        sv.transform<std::shared_ptr<T>>(), std::distance(sv.ss, sv.c_str()),
        sv.length(), sv.choice_count(), sv.choice());

    for (auto node : ast->nodes) {
      node->parent = ast;
    }
    return ast;
  };
}