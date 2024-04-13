  std::vector<T> transform(size_t beg = 0,
                           size_t end = static_cast<size_t>(-1)) const {
    std::vector<T> r;
    end = (std::min)(end, size());
    for (size_t i = beg; i < end; i++) {
      r.emplace_back(any_cast<T>((*this)[i]));
    }
    return r;
  }