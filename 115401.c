  void shift_capture_values() {
    assert(capture_scope_stack.size() >= 2);
    auto curr = &capture_scope_stack[capture_scope_stack_size - 1];
    auto prev = curr - 1;
    for (const auto &kv : *curr) {
      (*prev)[kv.first] = kv.second;
    }
  }