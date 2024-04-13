  void write(BasicWriter<Char> &w, const Char *start, const Char *end) {
    if (start != end)
      w << BasicStringRef<Char>(start, internal::to_unsigned(end - start));
  }