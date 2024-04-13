  void write(BasicCStringRef<Char> format, ArgList args) {
    BasicFormatter<Char>(args, *this).format(format);
  }