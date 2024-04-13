inline internal::Arg BasicFormatter<Char, AF>::parse_arg_index(const Char *&s) {
  const char *error = FMT_NULL;
  internal::Arg arg = *s < '0' || *s > '9' ?
        next_arg(error) : get_arg(internal::parse_nonnegative_int(s), error);
  if (error) {
    FMT_THROW(FormatError(
                *s != '}' && *s != ':' ? "invalid format string" : error));
  }
  return arg;
}