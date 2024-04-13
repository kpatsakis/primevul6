TEST(FormatterTest, FormatInfinity) {
  double inf = std::numeric_limits<double>::infinity();
  EXPECT_EQ("inf", format("{}", inf));
  EXPECT_EQ("+inf", format("{:+}", inf));
  EXPECT_EQ("-inf", format("{}", -inf));
  EXPECT_EQ(" inf", format("{: }", inf));
  EXPECT_EQ("INF", format("{:F}", inf));
  EXPECT_EQ("inf    ", format("{:<7}", inf));
  EXPECT_EQ("  inf  ", format("{:^7}", inf));
  EXPECT_EQ("    inf", format("{:>7}", inf));
}