TEST(FormatterTest, FormatWChar) {
  EXPECT_EQ(L"a", format(L"{0}", L'a'));
  // This shouldn't compile:
  //format("{}", L'a');
}