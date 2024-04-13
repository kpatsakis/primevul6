  void visit_custom(internal::Arg::CustomValue c) {
    c.format(&formatter_, c.value, &format_);
  }