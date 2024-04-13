  BasicWriter &operator<<(double value) {
    write_double(value, FormatSpec());
    return *this;
  }