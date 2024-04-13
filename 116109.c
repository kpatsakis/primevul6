  std::basic_string<Char> str() const {
    return std::basic_string<Char>(&buffer_[0], buffer_.size());
  }