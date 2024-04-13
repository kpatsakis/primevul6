  explicit operator std::basic_string_view<Char>() const FMT_NOEXCEPT {
    return std::basic_string_view<Char>(data_, size_);
  }