  BasicWriter &operator<<(ULongLong value) {
    return *this << IntFormatSpec<ULongLong>(value);
  }