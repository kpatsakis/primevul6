  void Reset() {
    if (on_heap_) {
      delete[] str_;
      on_heap_ = false;
    }

    str_ = NULL;
    size_ = 0;
  }