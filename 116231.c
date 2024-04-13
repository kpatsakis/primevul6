TEST(FormatterTest, FormatUnsignedChar) {
  EXPECT_EQ("42", format("{}", static_cast<unsigned char>(42)));
  EXPECT_EQ("42", format("{}", static_cast<uint8_t>(42)));
}