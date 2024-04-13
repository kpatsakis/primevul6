void Buffer<T>::append(const U *begin, const U *end) {
  FMT_ASSERT(end >= begin, "negative value");
  std::size_t new_size = size_ + static_cast<std::size_t>(end - begin);
  if (new_size > capacity_)
    grow(new_size);
  std::uninitialized_copy(begin, end,
                          internal::make_ptr(ptr_, capacity_) + size_);
  size_ = new_size;
}