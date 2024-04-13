inline internal::Arg BasicFormatter<Char, AF>::parse_arg_name(const Char *&s) {
  assert(internal::is_name_start(*s));
  const Char *start = s;
  Char c;
  do {
    c = *++s;
  } while (internal::is_name_start(c) || ('0' <= c && c <= '9'));
  const char *error = FMT_NULL;
  internal::Arg arg = get_arg(BasicStringRef<Char>(start, s - start), error);
  if (error)
    FMT_THROW(FormatError(error));
  return arg;
}