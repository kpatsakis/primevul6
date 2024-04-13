TEST(FormatterTest, FormatInt) {
  EXPECT_THROW_MSG(format("{0:v", 42),
      FormatError, "missing '}' in format string");
  check_unknown_types(42, "bBdoxXn", "integer");
}