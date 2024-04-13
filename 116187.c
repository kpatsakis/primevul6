TEST(FormatterTest, EmptySpecs) {
  EXPECT_EQ("42", format("{0:}", 42));
}