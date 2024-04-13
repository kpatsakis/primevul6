TEST(FormatterTest, FormatIntLocale) {
  ScopedMock<LocaleMock> mock;
  lconv lc = lconv();
  char sep[] = "--";
  lc.thousands_sep = sep;
  EXPECT_CALL(mock, localeconv()).Times(3).WillRepeatedly(testing::Return(&lc));
  EXPECT_EQ("123", format("{:n}", 123));
  EXPECT_EQ("1--234", format("{:n}", 1234));
  EXPECT_EQ("1--234--567", format("{:n}", 1234567));
}