  std::size_t size() const {
    return internal::to_unsigned(buffer_ - str_ + BUFFER_SIZE - 1);
  }