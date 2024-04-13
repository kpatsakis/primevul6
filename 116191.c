  CharPtr grow_buffer(std::size_t n) {
    std::size_t size = buffer_.size();
    buffer_.resize(size + n);
    return internal::make_ptr(&buffer_[size], n);
  }