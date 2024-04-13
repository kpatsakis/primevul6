TEST(FormatterTest, ManyArgs) {
  EXPECT_EQ("19", TestFormat<20>::format("{19}"));
  EXPECT_THROW_MSG(TestFormat<20>::format("{20}"),
                   FormatError, "argument index out of range");
  EXPECT_THROW_MSG(TestFormat<21>::format("{21}"),
                   FormatError, "argument index out of range");
  enum { MAX_PACKED_ARGS = fmt::ArgList::MAX_PACKED_ARGS };
  std::string format_str = fmt::format("{{{}}}", MAX_PACKED_ARGS + 1);
  EXPECT_THROW_MSG(TestFormat<MAX_PACKED_ARGS>::format(format_str),
                   FormatError, "argument index out of range");
}