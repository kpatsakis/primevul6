size_t QuotedString::printTo(const char *s, Print &p) {
  if (!s) return p.print("null");

  size_t n = p.write('\"');

  while (*s) {
    n += printCharTo(*s++, p);
  }

  return n + p.write('\"');
}