  ::testing::AssertionResult operator()(const char *, const T &value) const {
    return check_write<Char>(value, "char");
  }