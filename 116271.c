TEST(FormatterTest, FormatBin) {
  EXPECT_EQ("0", format("{0:b}", 0));
  EXPECT_EQ("101010", format("{0:b}", 42));
  EXPECT_EQ("101010", format("{0:b}", 42u));
  EXPECT_EQ("-101010", format("{0:b}", -42));
  EXPECT_EQ("11000000111001", format("{0:b}", 12345));
  EXPECT_EQ("10010001101000101011001111000", format("{0:b}", 0x12345678));
  EXPECT_EQ("10010000101010111100110111101111", format("{0:b}", 0x90ABCDEF));
  EXPECT_EQ("11111111111111111111111111111111",
            format("{0:b}", std::numeric_limits<uint32_t>::max()));
}