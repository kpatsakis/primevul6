  Result visit_int(int value) {
    return FMT_DISPATCH(visit_any_int(value));
  }