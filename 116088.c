  char *format_decimal(ULongLong value) {
    char *buffer_end = buffer_ + BUFFER_SIZE - 1;
    while (value >= 100) {
      // Integer division is slow so do it for a group of two digits instead
      // of for every digit. The idea comes from the talk by Alexandrescu
      // "Three Optimization Tips for C++". See speed-test for a comparison.
      unsigned index = static_cast<unsigned>((value % 100) * 2);
      value /= 100;
      *--buffer_end = internal::Data::DIGITS[index + 1];
      *--buffer_end = internal::Data::DIGITS[index];
    }
    if (value < 10) {
      *--buffer_end = static_cast<char>('0' + value);
      return buffer_end;
    }
    unsigned index = static_cast<unsigned>(value * 2);
    *--buffer_end = internal::Data::DIGITS[index + 1];
    *--buffer_end = internal::Data::DIGITS[index];
    return buffer_end;
  }