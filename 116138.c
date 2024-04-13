  BasicWriter &operator<<(
      typename internal::WCharHelper<StringRef, Char>::Supported value) {
    const char *str = value.data();
    buffer_.append(str, str + value.size());
    return *this;
  }