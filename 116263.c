TEST(FormatterTest, FormatShort) {
  short s = 42;
  EXPECT_EQ("42", format("{0:d}", s));
  unsigned short us = 42;
  EXPECT_EQ("42", format("{0:d}", us));
}