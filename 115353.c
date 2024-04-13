  const std::vector<std::shared_ptr<Ope>> &top_args() const {
    return args_stack[args_stack.size() - 1];
  }