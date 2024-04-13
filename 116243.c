  Result visit(const Arg &arg) {
    switch (arg.type) {
    case Arg::NONE:
    case Arg::NAMED_ARG:
      FMT_ASSERT(false, "invalid argument type");
      break;
    case Arg::INT:
      return FMT_DISPATCH(visit_int(arg.int_value));
    case Arg::UINT:
      return FMT_DISPATCH(visit_uint(arg.uint_value));
    case Arg::LONG_LONG:
      return FMT_DISPATCH(visit_long_long(arg.long_long_value));
    case Arg::ULONG_LONG:
      return FMT_DISPATCH(visit_ulong_long(arg.ulong_long_value));
    case Arg::BOOL:
      return FMT_DISPATCH(visit_bool(arg.int_value != 0));
    case Arg::CHAR:
      return FMT_DISPATCH(visit_char(arg.int_value));
    case Arg::DOUBLE:
      return FMT_DISPATCH(visit_double(arg.double_value));
    case Arg::LONG_DOUBLE:
      return FMT_DISPATCH(visit_long_double(arg.long_double_value));
    case Arg::CSTRING:
      return FMT_DISPATCH(visit_cstring(arg.string.value));
    case Arg::STRING:
      return FMT_DISPATCH(visit_string(arg.string));
    case Arg::WSTRING:
      return FMT_DISPATCH(visit_wstring(arg.wstring));
    case Arg::POINTER:
      return FMT_DISPATCH(visit_pointer(arg.pointer));
    case Arg::CUSTOM:
      return FMT_DISPATCH(visit_custom(arg.custom));
    }
    return Result();
  }