  void visit_cstring(const char *value) {
    if (spec_.type_ == 'p')
      return write_pointer(value);
    write(value);
  }