  BasicMemoryWriter &operator=(BasicMemoryWriter &&other) {
    buffer_ = std::move(other.buffer_);
    return *this;
  }