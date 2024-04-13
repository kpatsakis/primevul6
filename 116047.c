  BasicStringRef(const Char *s)
    : data_(s), size_(std::char_traits<Char>::length(s)) {}