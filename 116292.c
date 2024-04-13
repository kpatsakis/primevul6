TEST(FormatterTest, FormatString) {
  EXPECT_EQ("test", format("{0}", std::string("test")));
}