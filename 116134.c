  BasicStringRef(
	  const std::experimental::basic_string_view<Char, std::char_traits<Char>> &s)
	  : data_(s.data()), size_(s.size()) {}