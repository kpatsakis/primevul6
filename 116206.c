BasicWriter<Char> &operator<<(BasicWriter<Char> &f, const Date &d) {
  return f << d.year() << '-' << d.month() << '-' << d.day();
}