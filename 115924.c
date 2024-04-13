TEST(FormatterTest, RuntimePrecision) {
  char format_str[BUFFER_SIZE];
  safe_sprintf(format_str, "{0:.{%u", UINT_MAX);
  increment(format_str + 5);
  EXPECT_THROW_MSG(format(format_str, 0), FormatError, "number is too big");
  std::size_t size = std::strlen(format_str);
  format_str[size] = '}';
  format_str[size + 1] = 0;
  EXPECT_THROW_MSG(format(format_str, 0), FormatError, "number is too big");
  format_str[size + 1] = '}';
  format_str[size + 2] = 0;
  EXPECT_THROW_MSG(format(format_str, 0), FormatError, "number is too big");

  EXPECT_THROW_MSG(format("{0:.{", 0),
      FormatError, "invalid format string");
  EXPECT_THROW_MSG(format("{0:.{}", 0),
      FormatError, "cannot switch from manual to automatic argument indexing");
  EXPECT_THROW_MSG(format("{0:.{?}}", 0),
      FormatError, "invalid format string");
  EXPECT_THROW_MSG(format("{0:.{1}", 0, 0),
      FormatError, "precision not allowed in integer format specifier");
  EXPECT_THROW_MSG(format("{0:.{1}}", 0),
      FormatError, "argument index out of range");

  EXPECT_THROW_MSG(format("{0:.{0:}}", 0),
      FormatError, "invalid format string");

  EXPECT_THROW_MSG(format("{0:.{1}}", 0, -1),
      FormatError, "negative precision");
  EXPECT_THROW_MSG(format("{0:.{1}}", 0, (INT_MAX + 1u)),
      FormatError, "number is too big");
  EXPECT_THROW_MSG(format("{0:.{1}}", 0, -1l),
      FormatError, "negative precision");
  if (fmt::internal::const_check(sizeof(long) > sizeof(int))) {
    long value = INT_MAX;
    EXPECT_THROW_MSG(format("{0:.{1}}", 0, (value + 1)),
        FormatError, "number is too big");
  }
  EXPECT_THROW_MSG(format("{0:.{1}}", 0, (INT_MAX + 1ul)),
      FormatError, "number is too big");

  EXPECT_THROW_MSG(format("{0:.{1}}", 0, '0'),
      FormatError, "precision is not integer");
  EXPECT_THROW_MSG(format("{0:.{1}}", 0, 0.0),
      FormatError, "precision is not integer");

  EXPECT_THROW_MSG(format("{0:.{1}}", 42, 2),
      FormatError, "precision not allowed in integer format specifier");
  EXPECT_THROW_MSG(format("{0:.{1}f}", 42, 2),
      FormatError, "precision not allowed in integer format specifier");
  EXPECT_THROW_MSG(format("{0:.{1}}", 42u, 2),
      FormatError, "precision not allowed in integer format specifier");
  EXPECT_THROW_MSG(format("{0:.{1}f}", 42u, 2),
      FormatError, "precision not allowed in integer format specifier");
  EXPECT_THROW_MSG(format("{0:.{1}}", 42l, 2),
      FormatError, "precision not allowed in integer format specifier");
  EXPECT_THROW_MSG(format("{0:.{1}f}", 42l, 2),
      FormatError, "precision not allowed in integer format specifier");
  EXPECT_THROW_MSG(format("{0:.{1}}", 42ul, 2),
      FormatError, "precision not allowed in integer format specifier");
  EXPECT_THROW_MSG(format("{0:.{1}f}", 42ul, 2),
      FormatError, "precision not allowed in integer format specifier");
  EXPECT_THROW_MSG(format("{0:.{1}}", 42ll, 2),
      FormatError, "precision not allowed in integer format specifier");
  EXPECT_THROW_MSG(format("{0:.{1}f}", 42ll, 2),
      FormatError, "precision not allowed in integer format specifier");
  EXPECT_THROW_MSG(format("{0:.{1}}", 42ull, 2),
      FormatError, "precision not allowed in integer format specifier");
  EXPECT_THROW_MSG(format("{0:.{1}f}", 42ull, 2),
      FormatError, "precision not allowed in integer format specifier");
  EXPECT_THROW_MSG(format("{0:3.{1}}", 'x', 0),
      FormatError, "precision not allowed in integer format specifier");
  EXPECT_EQ("1.2", format("{0:.{1}}", 1.2345, 2));
  EXPECT_EQ("1.2", format("{1:.{0}}", 2, 1.2345l));

  EXPECT_THROW_MSG(format("{0:.{1}}", reinterpret_cast<void*>(0xcafe), 2),
      FormatError, "precision not allowed in pointer format specifier");
  EXPECT_THROW_MSG(format("{0:.{1}f}", reinterpret_cast<void*>(0xcafe), 2),
      FormatError, "precision not allowed in pointer format specifier");

  EXPECT_EQ("st", format("{0:.{1}}", "str", 2));
}