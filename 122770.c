  inline size_t remaining() const {
    return size_t(buffer_.get() + bufferSize_ - ptr_);
  }