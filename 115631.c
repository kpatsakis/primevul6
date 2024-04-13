  void push_capture_scope() {
    assert(capture_scope_stack_size <= capture_scope_stack.size());
    if (capture_scope_stack_size == capture_scope_stack.size()) {
      capture_scope_stack.emplace_back(std::map<std::string, std::string>());
    } else {
      auto &cs = capture_scope_stack[capture_scope_stack_size];
      if (!cs.empty()) { cs.clear(); }
    }
    capture_scope_stack_size++;
  }