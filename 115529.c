  void push_args(std::vector<std::shared_ptr<Ope>> &&args) {
    args_stack.emplace_back(args);
  }