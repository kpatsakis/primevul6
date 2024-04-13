  bool is_zom() const {
    return min_ == 0 && max_ == std::numeric_limits<size_t>::max();
  }