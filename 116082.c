inline void format_decimal(Char *buffer, UInt value, unsigned num_digits,
                           ThousandsSep thousands_sep) {
  buffer += num_digits;
  while (value >= 100) {
    // Integer division is slow so do it for a group of two digits instead
    // of for every digit. The idea comes from the talk by Alexandrescu
    // "Three Optimization Tips for C++". See speed-test for a comparison.
    unsigned index = static_cast<unsigned>((value % 100) * 2);
    value /= 100;
    *--buffer = Data::DIGITS[index + 1];
    thousands_sep(buffer);
    *--buffer = Data::DIGITS[index];
    thousands_sep(buffer);
  }
  if (value < 10) {
    *--buffer = static_cast<char>('0' + value);
    return;
  }
  unsigned index = static_cast<unsigned>(value * 2);
  *--buffer = Data::DIGITS[index + 1];
  thousands_sep(buffer);
  *--buffer = Data::DIGITS[index];
}