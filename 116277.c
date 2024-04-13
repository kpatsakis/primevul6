  void FormatSigned(LongLong value) {
    ULongLong abs_value = static_cast<ULongLong>(value);
    bool negative = value < 0;
    if (negative)
      abs_value = 0 - abs_value;
    str_ = format_decimal(abs_value);
    if (negative)
      *--str_ = '-';
  }