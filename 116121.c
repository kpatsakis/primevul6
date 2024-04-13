  BasicWriter &operator<<(const StrFormatSpec<StrChar> &spec) {
    const StrChar *s = spec.str();
    write_str(s, std::char_traits<Char>::length(s), spec);
    return *this;
  }