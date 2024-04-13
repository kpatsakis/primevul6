  Result visit_unhandled_arg() {
    FMT_DISPATCH(report_unhandled_arg());
    return Result();
  }