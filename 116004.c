  explicit FormatError(CStringRef message)
  : std::runtime_error(message.c_str()) {}