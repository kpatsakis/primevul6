  bool check_no_auto_index(const char *&error) {
    if (next_arg_index_ > 0) {
      error = "cannot switch from automatic to manual argument indexing";
      return false;
    }
    next_arg_index_ = -1;
    return true;
  }