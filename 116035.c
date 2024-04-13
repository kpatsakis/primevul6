  BasicWriter &operator<<(
      typename internal::WCharHelper<wchar_t, Char>::Supported value) {
    buffer_.push_back(value);
    return *this;
  }