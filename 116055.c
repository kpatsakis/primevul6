  Result visit_long_double(long double value) {
    return FMT_DISPATCH(visit_any_double(value));
  }