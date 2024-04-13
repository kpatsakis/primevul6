  operator fmt::LongLong() const {
    return fmt::LongLong(1) << 32;
  }