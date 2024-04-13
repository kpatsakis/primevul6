  static uint64_t type(unsigned long) {
    return sizeof(unsigned long) == sizeof(unsigned) ?
          Arg::UINT : Arg::ULONG_LONG;
  }