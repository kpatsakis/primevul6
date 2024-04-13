  void push_back(const T &value) {
    if (size_ == capacity_)
      grow(size_ + 1);
    ptr_[size_++] = value;
  }