  Result visit_ulong_long(ULongLong value) {
    return FMT_DISPATCH(visit_any_int(value));
  }