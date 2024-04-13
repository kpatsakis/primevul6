TEST(FormatterTest, FormatCStringRef) {
  EXPECT_EQ("test", format("{0}", CStringRef("test")));
}