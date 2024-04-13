  Result visit_char(int value) {
    return FMT_DISPATCH(visit_any_int(value));
  }