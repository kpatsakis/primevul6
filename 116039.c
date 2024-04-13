TEST(FormatterTest, Precision) {
  char format_str[BUFFER_SIZE];
  safe_sprintf(format_str, "{0:.%u", UINT_MAX);
  increment(format_str + 4);
  EXPECT_THROW_MSG(format(format_str, 0), FormatError, "number is too big");
  std::size_t size = std::strlen(format_str);
  format_str[size] = '}';
  format_str[size + 1] = 0;
  EXPECT_THROW_MSG(format(format_str, 0), FormatError, "number is too big");

  safe_sprintf(format_str, "{0:.%u", INT_MAX + 1u);
  EXPECT_THROW_MSG(format(format_str, 0), FormatError, "number is too big");
  safe_sprintf(format_str, "{0:.%u}", INT_MAX + 1u);
  EXPECT_THROW_MSG(format(format_str, 0), FormatError, "number is too big");

  EXPECT_THROW_MSG(format("{0:.", 0),
      FormatError, "missing precision specifier");
  EXPECT_THROW_MSG(format("{0:.}", 0),
      FormatError, "missing precision specifier");

  EXPECT_THROW_MSG(format("{0:.2", 0),
      FormatError, "precision not allowed in integer format specifier");
  EXPECT_THROW_MSG(format("{0:.2}", 42),
      FormatError, "precision not allowed in integer format specifier");
  EXPECT_THROW_MSG(format("{0:.2f}", 42),
      FormatError, "precision not allowed in integer format specifier");
  EXPECT_THROW_MSG(format("{0:.2}", 42u),
      FormatError, "precision not allowed in integer format specifier");
  EXPECT_THROW_MSG(format("{0:.2f}", 42u),
      FormatError, "precision not allowed in integer format specifier");
  EXPECT_THROW_MSG(format("{0:.2}", 42l),
      FormatError, "precision not allowed in integer format specifier");
  EXPECT_THROW_MSG(format("{0:.2f}", 42l),
      FormatError, "precision not allowed in integer format specifier");
  EXPECT_THROW_MSG(format("{0:.2}", 42ul),
      FormatError, "precision not allowed in integer format specifier");
  EXPECT_THROW_MSG(format("{0:.2f}", 42ul),
      FormatError, "precision not allowed in integer format specifier");
  EXPECT_THROW_MSG(format("{0:.2}", 42ll),
      FormatError, "precision not allowed in integer format specifier");
  EXPECT_THROW_MSG(format("{0:.2f}", 42ll),
      FormatError, "precision not allowed in integer format specifier");
  EXPECT_THROW_MSG(format("{0:.2}", 42ull),
      FormatError, "precision not allowed in integer format specifier");
  EXPECT_THROW_MSG(format("{0:.2f}", 42ull),
      FormatError, "precision not allowed in integer format specifier");
  EXPECT_THROW_MSG(format("{0:3.0}", 'x'),
      FormatError, "precision not allowed in integer format specifier");
  EXPECT_EQ("1.2", format("{0:.2}", 1.2345));
  EXPECT_EQ("1.2", format("{0:.2}", 1.2345l));

  EXPECT_THROW_MSG(format("{0:.2}", reinterpret_cast<void*>(0xcafe)),
      FormatError, "precision not allowed in pointer format specifier");
  EXPECT_THROW_MSG(format("{0:.2f}", reinterpret_cast<void*>(0xcafe)),
      FormatError, "precision not allowed in pointer format specifier");

  EXPECT_EQ("st", format("{0:.2}", "str"));
}