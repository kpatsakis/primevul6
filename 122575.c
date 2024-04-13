  void Save() {
    if (!on_heap_ && size_ > 0) {
      char* s = new char[size_];
      memcpy(s, str_, size_);
      str_ = s;
      on_heap_ = true;
    }
  }