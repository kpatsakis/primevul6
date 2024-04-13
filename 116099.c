  explicit operator std::experimental::basic_string_view<Char>() const FMT_NOEXCEPT {
	  return std::experimental::basic_string_view<Char>(data_, size_);
  }