TEST(FormatterTest, FormatCustom) {
  Date date(2012, 12, 9);
  EXPECT_THROW_MSG(fmt::format("{:s}", date), FormatError,
                   "unmatched '}' in format string");
}