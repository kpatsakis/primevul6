  SemanticValues &push() {
    assert(value_stack_size <= value_stack.size());
    if (value_stack_size == value_stack.size()) {
      value_stack.emplace_back(std::make_shared<SemanticValues>());
    } else {
      auto &sv = *value_stack[value_stack_size];
      if (!sv.empty()) {
        sv.clear();
        if (!sv.tags.empty()) { sv.tags.clear(); }
      }
      sv.s_ = nullptr;
      sv.n_ = 0;
      sv.choice_count_ = 0;
      sv.choice_ = 0;
      if (!sv.tokens.empty()) { sv.tokens.clear(); }
    }

    auto &sv = *value_stack[value_stack_size++];
    sv.path = path;
    sv.ss = s;
    sv.source_line_index = &source_line_index;
    return sv;
  }