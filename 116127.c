  ::testing::AssertionResult operator()(const char *, const T &value) const {
    ::testing::AssertionResult result = check_write<char>(value, "char");
    return result ? check_write<wchar_t>(value, "wchar_t") : result;
  }