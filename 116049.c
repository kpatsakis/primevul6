  MakeValue(unsigned long value) {
    if (const_check(sizeof(unsigned long) == sizeof(unsigned)))
      uint_value = static_cast<unsigned>(value);
    else
      ulong_long_value = value;
  }