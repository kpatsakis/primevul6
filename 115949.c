TEST(FormatterTest, UnmatchedBraces) {
  EXPECT_THROW_MSG(format("{"), FormatError, "invalid format string");
  EXPECT_THROW_MSG(format("}"), FormatError, "unmatched '}' in format string");
  EXPECT_THROW_MSG(format("{0{}"), FormatError, "invalid format string");
}