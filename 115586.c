  void enable_trace(TracerEnter tracer_enter, TracerLeave tracer_leave) {
    if (grammar_ != nullptr) {
      auto &rule = (*grammar_)[start_];
      rule.tracer_enter = tracer_enter;
      rule.tracer_leave = tracer_leave;
    }
  }