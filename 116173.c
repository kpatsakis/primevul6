unsigned parse_nonnegative_int(const Char *&s) {
  assert('0' <= *s && *s <= '9');
  unsigned value = 0;
  // Convert to unsigned to prevent a warning.
  unsigned max_int = (std::numeric_limits<int>::max)();
  unsigned big = max_int / 10;
  do {
    // Check for overflow.
    if (value > big) {
      value = max_int + 1;
      break;
    }
    value = value * 10 + (*s - '0');
    ++s;
  } while ('0' <= *s && *s <= '9');
  // Convert to unsigned to prevent a warning.
  if (value > max_int)
    FMT_THROW(FormatError("number is too big"));
  return value;
}