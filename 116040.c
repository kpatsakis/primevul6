  int compare(BasicStringRef other) const {
    std::size_t size = size_ < other.size_ ? size_ : other.size_;
    int result = std::char_traits<Char>::compare(data_, other.data_, size);
    if (result == 0)
      result = size_ == other.size_ ? 0 : (size_ < other.size_ ? -1 : 1);
    return result;
  }