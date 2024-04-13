TEST(FormatterTest, RuntimeWidth) {
  char format_str[BUFFER_SIZE];
  safe_sprintf(format_str, "{0:{%u", UINT_MAX);
  increment(format_str + 4);
  EXPECT_THROW_MSG(format(format_str, 0), FormatError, "number is too big");
  std::size_t size = std::strlen(format_str);
  format_str[size] = '}';
  format_str[size + 1] = 0;
  EXPECT_THROW_MSG(format(format_str, 0), FormatError, "number is too big");
  format_str[size + 1] = '}';
  format_str[size + 2] = 0;
  EXPECT_THROW_MSG(format(format_str, 0), FormatError, "number is too big");

  EXPECT_THROW_MSG(format("{0:{", 0),
      FormatError, "invalid format string");
  EXPECT_THROW_MSG(format("{0:{}", 0),
      FormatError, "cannot switch from manual to automatic argument indexing");
  EXPECT_THROW_MSG(format("{0:{?}}", 0),
      FormatError, "invalid format string");
  EXPECT_THROW_MSG(format("{0:{1}}", 0),
      FormatError, "argument index out of range");

  EXPECT_THROW_MSG(format("{0:{0:}}", 0),
      FormatError, "invalid format string");

  EXPECT_THROW_MSG(format("{0:{1}}", 0, -1),
      FormatError, "negative width");
  EXPECT_THROW_MSG(format("{0:{1}}", 0, (INT_MAX + 1u)),
      FormatError, "number is too big");
  EXPECT_THROW_MSG(format("{0:{1}}", 0, -1l),
      FormatError, "negative width");
  if (fmt::internal::const_check(sizeof(long) > sizeof(int))) {
    long value = INT_MAX;
    EXPECT_THROW_MSG(format("{0:{1}}", 0, (value + 1)),
        FormatError, "number is too big");
  }
  EXPECT_THROW_MSG(format("{0:{1}}", 0, (INT_MAX + 1ul)),
      FormatError, "number is too big");

  EXPECT_THROW_MSG(format("{0:{1}}", 0, '0'),
      FormatError, "width is not integer");
  EXPECT_THROW_MSG(format("{0:{1}}", 0, 0.0),
      FormatError, "width is not integer");

  EXPECT_EQ(" -42", format("{0:{1}}", -42, 4));
  EXPECT_EQ("   42", format("{0:{1}}", 42u, 5));
  EXPECT_EQ("   -42", format("{0:{1}}", -42l, 6));
  EXPECT_EQ("     42", format("{0:{1}}", 42ul, 7));
  EXPECT_EQ("   -42", format("{0:{1}}", -42ll, 6));
  EXPECT_EQ("     42", format("{0:{1}}", 42ull, 7));
  EXPECT_EQ("   -1.23", format("{0:{1}}", -1.23, 8));
  EXPECT_EQ("    -1.23", format("{0:{1}}", -1.23l, 9));
  EXPECT_EQ("    0xcafe",
            format("{0:{1}}", reinterpret_cast<void*>(0xcafe), 10));
  EXPECT_EQ("x          ", format("{0:{1}}", 'x', 11));
  EXPECT_EQ("str         ", format("{0:{1}}", "str", 12));
}