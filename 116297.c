  BasicWriter &operator<<(long value) {
    write_decimal(value);
    return *this;
  }