TEST(FormatTest, FixedEnum) {
  EXPECT_EQ("0", fmt::format("{}", B));
}