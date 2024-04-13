TEST(FormatterTest, FormatDouble) {
  check_unknown_types(1.2, "eEfFgGaA", "double");
  EXPECT_EQ("0", format("{0:}", 0.0));
  EXPECT_EQ("0.000000", format("{0:f}", 0.0));
  EXPECT_EQ("392.65", format("{0:}", 392.65));
  EXPECT_EQ("392.65", format("{0:g}", 392.65));
  EXPECT_EQ("392.65", format("{0:G}", 392.65));
  EXPECT_EQ("392.650000", format("{0:f}", 392.65));
  EXPECT_EQ("392.650000", format("{0:F}", 392.65));
  char buffer[BUFFER_SIZE];
  safe_sprintf(buffer, "%e", 392.65);
  EXPECT_EQ(buffer, format("{0:e}", 392.65));
  safe_sprintf(buffer, "%E", 392.65);
  EXPECT_EQ(buffer, format("{0:E}", 392.65));
  EXPECT_EQ("+0000392.6", format("{0:+010.4g}", 392.65));
  safe_sprintf(buffer, "%a", -42.0);
  EXPECT_EQ(buffer, format("{:a}", -42.0));
  safe_sprintf(buffer, "%A", -42.0);
  EXPECT_EQ(buffer, format("{:A}", -42.0));
}