inline constexpr unsigned int operator"" _(const char *s, size_t) {
  return str2tag(s);
}