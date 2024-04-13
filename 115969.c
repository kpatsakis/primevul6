  BasicWriter &operator<<(LongLong value) {
    write_decimal(value);
    return *this;
  }