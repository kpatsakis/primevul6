inline internal::Arg BasicFormatter<Char, AF>::get_arg(
    BasicStringRef<Char> arg_name, const char *&error) {
  if (check_no_auto_index(error)) {
    map_.init(args());
    const internal::Arg *arg = map_.find(arg_name);
    if (arg)
      return *arg;
    error = "argument not found";
  }
  return internal::Arg();
}