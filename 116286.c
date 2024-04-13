void ArgMap<Char>::init(const ArgList &args) {
  if (!map_.empty())
    return;
  typedef internal::NamedArg<Char> NamedArg;
  const NamedArg *named_arg = FMT_NULL;
  bool use_values =
      args.type(ArgList::MAX_PACKED_ARGS - 1) == internal::Arg::NONE;
  if (use_values) {
    for (unsigned i = 0;/*nothing*/; ++i) {
      internal::Arg::Type arg_type = args.type(i);
      switch (arg_type) {
      case internal::Arg::NONE:
        return;
      case internal::Arg::NAMED_ARG:
        named_arg = static_cast<const NamedArg*>(args.values_[i].pointer);
        map_.push_back(Pair(named_arg->name, *named_arg));
        break;
      default:
        /*nothing*/;
      }
    }
    return;
  }
  for (unsigned i = 0; i != ArgList::MAX_PACKED_ARGS; ++i) {
    internal::Arg::Type arg_type = args.type(i);
    if (arg_type == internal::Arg::NAMED_ARG) {
      named_arg = static_cast<const NamedArg*>(args.args_[i].pointer);
      map_.push_back(Pair(named_arg->name, *named_arg));
    }
  }
  for (unsigned i = ArgList::MAX_PACKED_ARGS;/*nothing*/; ++i) {
    switch (args.args_[i].type) {
    case internal::Arg::NONE:
      return;
    case internal::Arg::NAMED_ARG:
      named_arg = static_cast<const NamedArg*>(args.args_[i].pointer);
      map_.push_back(Pair(named_arg->name, *named_arg));
      break;
    default:
      /*nothing*/;
    }
  }
}