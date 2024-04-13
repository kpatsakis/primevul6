TEST(LiteralsTest, Format) {
  auto udl_format = "{}c{}"_format("ab", 1);
  EXPECT_EQ(format("{}c{}", "ab", 1), udl_format);
  auto udl_format_w = L"{}c{}"_format(L"ab", 1);
  EXPECT_EQ(format(L"{}c{}", L"ab", 1), udl_format_w);
}