void ast_to_s_core(const std::shared_ptr<T> &ptr, std::string &s, int level,
                   std::function<std::string(const T &ast, int level)> fn) {
  const auto &ast = *ptr;
  for (auto i = 0; i < level; i++) {
    s += "  ";
  }
  auto name = ast.original_name;
  if (ast.original_choice_count > 0) {
    name += "/" + std::to_string(ast.original_choice);
  }
  if (ast.name != ast.original_name) { name += "[" + ast.name + "]"; }
  if (ast.is_token) {
    s += "- " + name + " (" + ast.token + ")\n";
  } else {
    s += "+ " + name + "\n";
  }
  if (fn) { s += fn(ast, level + 1); }
  for (auto node : ast.nodes) {
    ast_to_s_core(node, s, level + 1, fn);
  }
}