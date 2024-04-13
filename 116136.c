TEST(FormatterTest, Width) {
  char format_str[BUFFER_SIZE];
  safe_sprintf(format_str, "{0:%u", UINT_MAX);
  increment(format_str + 3);
  EXPECT_THROW_MSG(format(format_str, 0), FormatError, "number is too big");
  std::size_t size = std::strlen(format_str);
  format_str[size] = '}';
  format_str[size + 1] = 0;
  EXPECT_THROW_MSG(format(format_str, 0), FormatError, "number is too big");

  safe_sprintf(format_str, "{0:%u", INT_MAX + 1u);
  EXPECT_THROW_MSG(format(format_str, 0), FormatError, "number is too big");
  safe_sprintf(format_str, "{0:%u}", INT_MAX + 1u);
  EXPECT_THROW_MSG(format(format_str, 0), FormatError, "number is too big");
  EXPECT_EQ(" -42", format("{0:4}", -42));
  EXPECT_EQ("   42", format("{0:5}", 42u));
  EXPECT_EQ("   -42", format("{0:6}", -42l));
  EXPECT_EQ("     42", format("{0:7}", 42ul));
  EXPECT_EQ("   -42", format("{0:6}", -42ll));
  EXPECT_EQ("     42", format("{0:7}", 42ull));
  EXPECT_EQ("   -1.23", format("{0:8}", -1.23));
  EXPECT_EQ("    -1.23", format("{0:9}", -1.23l));
  EXPECT_EQ("    0xcafe", format("{0:10}", reinterpret_cast<void*>(0xcafe)));
  EXPECT_EQ("x          ", format("{0:11}", 'x'));
  EXPECT_EQ("str         ", format("{0:12}", "str"));
}