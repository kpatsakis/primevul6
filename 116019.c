  BasicFormatter(const ArgList &args, BasicWriter<Char> &w)
    : internal::FormatterBase(args), writer_(w) {}