TEST(FormatterTest, FormatFloat) {
  EXPECT_EQ("392.500000", format("{0:f}", 392.5f));
}