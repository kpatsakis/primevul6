  NamedArg(BasicStringRef<Char> argname, const T &value)
  : Arg(MakeArg< BasicFormatter<Char> >(value)), name(argname) {}