TEST(FormatTest, ConstFormatMessage) {
  test_class c;
  EXPECT_EQ("[42] something happened",
    c.format_message(42, "{} happened", "something"));
}