  std::basic_string<Char> to_string() const {
    return std::basic_string<Char>(data_, size_);
  }