  explicit scope_exit(EF &&f)
      : exit_function(std::move(f)), execute_on_destruction{true} {}