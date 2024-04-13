  BasicStringRef(
      const std::basic_string<Char, std::char_traits<Char>, Allocator> &s)
  : data_(s.c_str()), size_(s.size()) {}