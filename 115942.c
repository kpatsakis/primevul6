  MakeArg(const T &value)
  : Arg(MakeValue<Formatter>(value)) {
    type = static_cast<Arg::Type>(MakeValue<Formatter>::type(value));
  }