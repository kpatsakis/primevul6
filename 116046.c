TEST(FormatterTest, FormatStringRef) {
  EXPECT_EQ("test", format("{0}", StringRef("test")));
}