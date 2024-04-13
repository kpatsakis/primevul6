  Result visit_cstring(const char *) {
    return FMT_DISPATCH(visit_unhandled_arg());
  }