TEST(FormatterTest, CenterAlign) {
  EXPECT_EQ(" 42  ", format("{0:^5}", 42));
  EXPECT_EQ(" 42  ", format("{0:^5o}", 042));
  EXPECT_EQ(" 42  ", format("{0:^5x}", 0x42));
  EXPECT_EQ(" -42 ", format("{0:^5}", -42));
  EXPECT_EQ(" 42  ", format("{0:^5}", 42u));
  EXPECT_EQ(" -42 ", format("{0:^5}", -42l));
  EXPECT_EQ(" 42  ", format("{0:^5}", 42ul));
  EXPECT_EQ(" -42 ", format("{0:^5}", -42ll));
  EXPECT_EQ(" 42  ", format("{0:^5}", 42ull));
  EXPECT_EQ(" -42  ", format("{0:^6}", -42.0));
  EXPECT_EQ(" -42 ", format("{0:^5}", -42.0l));
  EXPECT_EQ("  c  ", format("{0:^5}", 'c'));
  EXPECT_EQ(" abc  ", format("{0:^6}", "abc"));
  EXPECT_EQ(" 0xface ", format("{0:^8}", reinterpret_cast<void*>(0xface)));
}