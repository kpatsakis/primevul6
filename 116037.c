  BasicWriter &operator<<(long double value) {
    write_double(value, FormatSpec());
    return *this;
  }