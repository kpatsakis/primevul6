TEST(FormatterTest, FormatUCharString) {
  unsigned char str[] = "test";
  EXPECT_EQ("test", format("{0:s}", str));
  const unsigned char *const_str = str;
  EXPECT_EQ("test", format("{0:s}", const_str));
  unsigned char *ptr = str;
  EXPECT_EQ("test", format("{0:s}", ptr));
}