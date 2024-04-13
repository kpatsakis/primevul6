  Result visit_long_long(LongLong value) {
    return FMT_DISPATCH(visit_any_int(value));
  }