TEST(FormatterTest, FormatLongDouble) {
  EXPECT_EQ("0", format("{0:}", 0.0l));
  EXPECT_EQ("0.000000", format("{0:f}", 0.0l));
  EXPECT_EQ("392.65", format("{0:}", 392.65l));
  EXPECT_EQ("392.65", format("{0:g}", 392.65l));
  EXPECT_EQ("392.65", format("{0:G}", 392.65l));
  EXPECT_EQ("392.650000", format("{0:f}", 392.65l));
  EXPECT_EQ("392.650000", format("{0:F}", 392.65l));
  char buffer[BUFFER_SIZE];
  safe_sprintf(buffer, "%Le", 392.65l);
  EXPECT_EQ(buffer, format("{0:e}", 392.65l));
  EXPECT_EQ("+0000392.6", format("{0:+010.4g}", 392.64l));
}