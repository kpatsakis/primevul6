TEST(FormatterTest, FormatConvertibleToLongLong) {
  EXPECT_EQ("100000000", format("{:x}", ConvertibleToLongLong()));
}