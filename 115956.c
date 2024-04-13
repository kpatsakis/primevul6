TEST(FormatterTest, FormatChar) {
  const char types[] = "cbBdoxXn";
  check_unknown_types('a', types, "char");
  EXPECT_EQ("a", format("{0}", 'a'));
  EXPECT_EQ("z", format("{0:c}", 'z'));
  EXPECT_EQ(L"a", format(L"{0}", 'a'));
  int n = 'x';
  for (const char *type = types + 1; *type; ++type) {
    std::string format_str = fmt::format("{{:{}}}", *type);
    EXPECT_EQ(fmt::format(format_str, n), fmt::format(format_str, 'x'));
  }
  EXPECT_EQ(fmt::format("{:02X}", n), fmt::format("{:02X}", 'x'));
}