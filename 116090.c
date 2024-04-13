TEST(FormatterTest, FormatStringView) {
  EXPECT_EQ("test", format("{0}", std::string_view("test")));
}