  void deallocate() {
    if (this->ptr_ != data_) Allocator::deallocate(this->ptr_, this->capacity_);
  }