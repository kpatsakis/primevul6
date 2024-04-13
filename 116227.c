  Result visit_any_int(T) {
    return FMT_DISPATCH(visit_unhandled_arg());
  }