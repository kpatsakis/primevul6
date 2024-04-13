void check_sign(const Char *&s, const Arg &arg) {
  char sign = static_cast<char>(*s);
  require_numeric_argument(arg, sign);
  if (arg.type == Arg::UINT || arg.type == Arg::ULONG_LONG) {
    FMT_THROW(FormatError(fmt::format(
      "format specifier '{}' requires signed argument", sign)));
  }
  ++s;
}