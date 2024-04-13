TEST(FormatterTest, WideFormatString) {
  EXPECT_EQ(L"42", format(L"{}", 42));
  EXPECT_EQ(L"4.2", format(L"{}", 4.2));
  EXPECT_EQ(L"abc", format(L"{}", L"abc"));
  EXPECT_EQ(L"z", format(L"{}", L'z'));
}