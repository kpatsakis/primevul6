  void write_decimal(Int value) {
    typedef typename internal::IntTraits<Int>::MainType MainType;
    MainType abs_value = static_cast<MainType>(value);
    if (internal::is_negative(value)) {
      abs_value = 0 - abs_value;
      *write_unsigned_decimal(abs_value, 1) = '-';
    } else {
      write_unsigned_decimal(abs_value, 0);
    }
  }