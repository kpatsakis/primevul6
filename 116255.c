TEST(FormatTest, Enum) {
  EXPECT_EQ("0", fmt::format("{}", A));
}