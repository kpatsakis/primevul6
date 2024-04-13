  void resize(std::size_t new_size) {
    if (new_size > capacity_)
      grow(new_size);
    size_ = new_size;
  }