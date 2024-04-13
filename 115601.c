  ~scope_exit() {
    if (execute_on_destruction) { this->exit_function(); }
  }