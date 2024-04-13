void MemoryBuffer<T, SIZE, Allocator>::grow(std::size_t size) {
  std::size_t new_capacity = this->capacity_ + this->capacity_ / 2;
  if (size > new_capacity)
      new_capacity = size;
#if FMT_USE_ALLOCATOR_TRAITS
  T *new_ptr =
      std::allocator_traits<Allocator>::allocate(*this, new_capacity, FMT_NULL);
#else
  T *new_ptr = this->allocate(new_capacity, FMT_NULL);
#endif
  // The following code doesn't throw, so the raw pointer above doesn't leak.
  std::uninitialized_copy(this->ptr_, this->ptr_ + this->size_,
                          make_ptr(new_ptr, new_capacity));
  std::size_t old_capacity = this->capacity_;
  T *old_ptr = this->ptr_;
  this->capacity_ = new_capacity;
  this->ptr_ = new_ptr;
  // deallocate may throw (at least in principle), but it doesn't matter since
  // the buffer already uses the new storage and will deallocate it in case
  // of exception.
  if (old_ptr != data_)
    Allocator::deallocate(old_ptr, old_capacity);
}