  Result visit_pointer(const void *) {
    return FMT_DISPATCH(visit_unhandled_arg());
  }