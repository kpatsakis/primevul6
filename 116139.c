TEST(FormatterTest, FormatSCharString) {
  signed char str[] = "test";
  EXPECT_EQ("test", format("{0:s}", str));
  const signed char *const_str = str;
  EXPECT_EQ("test", format("{0:s}", const_str));
}