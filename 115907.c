TEST(FormatTest, Variadic) {
  EXPECT_EQ("abc1", format("{}c{}", "ab", 1));
  EXPECT_EQ(L"abc1", format(L"{}c{}", L"ab", 1));
}