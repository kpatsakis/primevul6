  BasicWriter &operator<<(unsigned long value) {
    return *this << IntFormatSpec<unsigned long>(value);
  }