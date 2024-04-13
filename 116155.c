::testing::AssertionResult check_write(const T &value, const char *type) {
  std::basic_string<Char> actual =
      (fmt::BasicMemoryWriter<Char>() << value).str();
  std::basic_string<Char> expected;
  std_format(value, expected);
  if (expected == actual)
    return ::testing::AssertionSuccess();
  return ::testing::AssertionFailure()
      << "Value of: (Writer<" << type << ">() << value).str()\n"
      << "  Actual: " << actual << "\n"
      << "Expected: " << expected << "\n";
}