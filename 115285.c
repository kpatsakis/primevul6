  std::shared_ptr<T> optimize(std::shared_ptr<T> original,
                              std::shared_ptr<T> parent = nullptr) {
    auto found =
        std::find(rules_.begin(), rules_.end(), original->name) != rules_.end();
    bool opt = mode_ ? !found : found;

    if (opt && original->nodes.size() == 1) {
      auto child = optimize(original->nodes[0], parent);
      return std::make_shared<T>(*child, original->name.c_str(),
                                 original->choice_count, original->position,
                                 original->length, original->choice);
    }

    auto ast = std::make_shared<T>(*original);
    ast->parent = parent;
    ast->nodes.clear();
    for (auto node : original->nodes) {
      auto child = optimize(node, ast);
      ast->nodes.push_back(child);
    }
    return ast;
  }