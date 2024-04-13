  MemoryBuffer &operator=(MemoryBuffer &&other) {
    assert(this != &other);
    deallocate();
    move(other);
    return *this;
  }