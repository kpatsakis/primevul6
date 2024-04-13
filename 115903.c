  void write(const char *value) {
    Arg::StringValue<char> str = {value, value ? std::strlen(value) : 0};
    writer_.write_str(str, spec_);
  }