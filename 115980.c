TEST(BoolTest, FormatBool) {
  EXPECT_EQ("true", format("{}", true));
  EXPECT_EQ("false", format("{}", false));
  EXPECT_EQ("1", format("{:d}", true));
  EXPECT_EQ("true ", format("{:5}", true));
  EXPECT_EQ(L"true", format(L"{}", true));
}