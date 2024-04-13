  Result visit_uint(unsigned value) {
    return FMT_DISPATCH(visit_any_int(value));
  }