  BasicArgFormatter(BasicFormatter<Char, Impl> &formatter,
                    Spec &spec, const Char *fmt)
  : internal::ArgFormatterBase<Impl, Char, Spec>(formatter.writer(), spec),
    formatter_(formatter), format_(fmt) {}