  Result visit_wstring(Arg::StringValue<wchar_t>) {
    return FMT_DISPATCH(visit_unhandled_arg());
  }