  Result visit_bool(bool value) {
    return FMT_DISPATCH(visit_any_int(value));
  }