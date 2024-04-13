  Result visit_any_double(T) {
    return FMT_DISPATCH(visit_unhandled_arg());
  }