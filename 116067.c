TEST(FormatterTest, ArgErrors) {
  EXPECT_THROW_MSG(format("{"), FormatError, "invalid format string");
  EXPECT_THROW_MSG(format("{?}"), FormatError, "invalid format string");
  EXPECT_THROW_MSG(format("{0"), FormatError, "invalid format string");
  EXPECT_THROW_MSG(format("{0}"), FormatError, "argument index out of range");

  char format_str[BUFFER_SIZE];
  safe_sprintf(format_str, "{%u", INT_MAX);
  EXPECT_THROW_MSG(format(format_str), FormatError, "invalid format string");
  safe_sprintf(format_str, "{%u}", INT_MAX);
  EXPECT_THROW_MSG(format(format_str), FormatError,
      "argument index out of range");

  safe_sprintf(format_str, "{%u", INT_MAX + 1u);
  EXPECT_THROW_MSG(format(format_str), FormatError, "number is too big");
  safe_sprintf(format_str, "{%u}", INT_MAX + 1u);
  EXPECT_THROW_MSG(format(format_str), FormatError, "number is too big");
}