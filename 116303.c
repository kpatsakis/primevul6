void format_arg(fmt::BasicFormatter<Char, ArgFormatter> &f,
    const Char *&format_str, const ArgJoin<Char, It>& e) {
  const Char* end = format_str;
  int brace_level = 1;
  while (*end) {
    if (*end == '}' && --brace_level == 0)
      break;
    if (*end == '{')
      ++brace_level;
    ++end;
  }
  if (*end != '}')
    FMT_THROW(FormatError("missing '}' in format string"));

  It it = e.first;
  if (it != e.last) {
    const Char* save = format_str;
    f.format(format_str, internal::MakeArg<fmt::BasicFormatter<Char, ArgFormatter> >(*it++));
    while (it != e.last) {
      f.writer().write(e.sep);
      format_str = save;
      f.format(format_str, internal::MakeArg<fmt::BasicFormatter<Char, ArgFormatter> >(*it++));
    }
  }
  format_str = end + 1;
}