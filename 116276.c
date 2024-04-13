  StrFormatSpec(const Char *str, unsigned width, FillChar fill)
  : AlignSpec(width, fill), str_(str) {
    internal::CharTraits<Char>::convert(FillChar());
  }