void BasicFormatter<Char, AF>::format(BasicCStringRef<Char> format_str) {
  const Char *s = format_str.c_str();
  const Char *start = s;
  while (*s) {
    Char c = *s++;
    if (c != '{' && c != '}') continue;
    if (*s == c) {
      write(writer_, start, s);
      start = ++s;
      continue;
    }
    if (c == '}')
      FMT_THROW(FormatError("unmatched '}' in format string"));
    write(writer_, start, s - 1);
    internal::Arg arg = internal::is_name_start(*s) ?
          parse_arg_name(s) : parse_arg_index(s);
    start = s = format(s, arg);
  }
  write(writer_, start, s);
}