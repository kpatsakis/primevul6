  Result visit_string(Arg::StringValue<char>) {
    return FMT_DISPATCH(visit_unhandled_arg());
  }