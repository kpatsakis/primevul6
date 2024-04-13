  explicit FormatterBase(const ArgList &args) {
    args_ = args;
    next_arg_index_ = 0;
  }