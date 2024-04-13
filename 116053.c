  Result visit_custom(Arg::CustomValue) {
    return FMT_DISPATCH(visit_unhandled_arg());
  }