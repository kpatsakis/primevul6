  MakeValue(long value) {
    // To minimize the number of types we need to deal with, long is
    // translated either to int or to long long depending on its size.
    if (const_check(sizeof(long) == sizeof(int)))
      int_value = static_cast<int>(value);
    else
      long_long_value = value;
  }