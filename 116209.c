TEST(FormatterTest, FormatNaN) {
  double nan = std::numeric_limits<double>::quiet_NaN();
  EXPECT_EQ("nan", format("{}", nan));
  EXPECT_EQ("+nan", format("{:+}", nan));
  EXPECT_EQ(" nan", format("{: }", nan));
  EXPECT_EQ("NAN", format("{:F}", nan));
  EXPECT_EQ("nan    ", format("{:<7}", nan));
  EXPECT_EQ("  nan  ", format("{:^7}", nan));
  EXPECT_EQ("    nan", format("{:>7}", nan));
}