  const char *c_str() const {
    buffer_[BUFFER_SIZE - 1] = '\0';
    return str_;
  }