  BasicWriter &operator<<(int value) {
    write_decimal(value);
    return *this;
  }