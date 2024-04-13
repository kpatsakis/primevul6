TEST(FormatterTest, CustomFormat) {
  EXPECT_EQ("42", format("{0}", Answer()));
}