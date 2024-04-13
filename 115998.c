void format_arg(fmt::BasicFormatter<char> &f, const char *, const Date &d) {
  f.writer() << d.year() << '-' << d.month() << '-' << d.day();
}