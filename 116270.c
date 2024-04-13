TEST(FormatterTest, NamedArg) {
  EXPECT_EQ("1/a/A", format("{_1}/{a_}/{A_}", fmt::arg("a_", 'a'),
                            fmt::arg("A_", "A"), fmt::arg("_1", 1)));
  char a = 'A', b = 'B', c = 'C';
  EXPECT_EQ("BB/AA/CC", format("{1}{b}/{0}{a}/{2}{c}", FMT_CAPTURE(a, b, c)));
  EXPECT_EQ(" A", format("{a:>2}", FMT_CAPTURE(a)));
  EXPECT_THROW_MSG(format("{a+}", FMT_CAPTURE(a)), FormatError,
                   "missing '}' in format string");
  EXPECT_THROW_MSG(format("{a}"), FormatError, "argument not found");
  EXPECT_THROW_MSG(format("{d}", FMT_CAPTURE(a, b, c)), FormatError,
                   "argument not found");
  EXPECT_THROW_MSG(format("{a}{}", FMT_CAPTURE(a)),
    FormatError, "cannot switch from manual to automatic argument indexing");
  EXPECT_THROW_MSG(format("{}{a}", FMT_CAPTURE(a)),
    FormatError, "cannot switch from automatic to manual argument indexing");
  EXPECT_EQ(" -42", format("{0:{width}}", -42, fmt::arg("width", 4)));
  EXPECT_EQ("st", format("{0:.{precision}}", "str", fmt::arg("precision", 2)));
  int n = 100;
  EXPECT_EQ(L"n=100", format(L"n={n}", FMT_CAPTURE_W(n)));
}