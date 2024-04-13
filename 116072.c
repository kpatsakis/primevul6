  Result visit_double(double value) {
    return FMT_DISPATCH(visit_any_double(value));
  }