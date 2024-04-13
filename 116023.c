  static uint64_t type(long) {
    return sizeof(long) == sizeof(int) ? Arg::INT : Arg::LONG_LONG;
  }