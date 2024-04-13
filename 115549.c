  any &operator=(const any &rhs) {
    if (this != &rhs) {
      if (content_) { delete content_; }
      content_ = rhs.clone();
    }
    return *this;
  }