  BasicWriter &operator<<(unsigned value) {
    return *this << IntFormatSpec<unsigned>(value);
  }