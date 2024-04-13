  friend bool operator>(BasicStringRef lhs, BasicStringRef rhs) {
    return lhs.compare(rhs) > 0;
  }