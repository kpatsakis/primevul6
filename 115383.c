  any &operator=(any &&rhs) {
    if (this != &rhs) {
      if (content_) { delete content_; }
      content_ = rhs.content_;
      rhs.content_ = nullptr;
    }
    return *this;
  }