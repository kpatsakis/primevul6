  ArgFormatter(BasicFormatter<Char> &formatter,
               FormatSpec &spec, const Char *fmt)
  : BasicArgFormatter<ArgFormatter<Char>,
                      Char, FormatSpec>(formatter, spec, fmt) {}