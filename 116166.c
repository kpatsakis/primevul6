TEST(FormatTest, FormatMessageExample) {
  EXPECT_EQ("[42] something happened",
      format_message(42, "{} happened", "something"));
}