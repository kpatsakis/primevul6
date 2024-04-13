  void write(bool value) {
    const char *str_value = value ? "true" : "false";
    Arg::StringValue<char> str = { str_value, std::strlen(str_value) };
    writer_.write_str(str, spec_);
  }