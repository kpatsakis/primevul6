  static internal::Arg::Type type(uint64_t types, unsigned index) {
    unsigned shift = index * 4;
    uint64_t mask = 0xf;
    return static_cast<internal::Arg::Type>(
          (types & (mask << shift)) >> shift);
  }