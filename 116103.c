  BasicWriter &operator<<(fmt::BasicStringRef<Char> value) {
    const Char *str = value.data();
    buffer_.append(str, str + value.size());
    return *this;
  }