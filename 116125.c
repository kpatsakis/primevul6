  BasicWriter &operator<<(char value) {
    buffer_.push_back(value);
    return *this;
  }