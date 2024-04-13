  void move(MemoryBuffer &other) {
    Allocator &this_alloc = *this, &other_alloc = other;
    this_alloc = std::move(other_alloc);
    this->size_ = other.size_;
    this->capacity_ = other.capacity_;
    if (other.ptr_ == other.data_) {
      this->ptr_ = data_;
      std::uninitialized_copy(other.data_, other.data_ + this->size_,
                              make_ptr(data_, this->capacity_));
    } else {
      this->ptr_ = other.ptr_;
      // Set pointer to the inline array so that delete is not called
      // when deallocating.
      other.ptr_ = other.data_;
    }
  }