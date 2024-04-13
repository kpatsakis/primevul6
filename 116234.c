  Arg get_arg(unsigned arg_index, const char *&error) {
    return check_no_auto_index(error) ? do_get_arg(arg_index, error) : Arg();
  }