  void reserve(std::size_t capacity) {
    if (capacity > capacity_)
      grow(capacity);
  }