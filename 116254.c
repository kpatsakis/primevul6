TEST(FormatterTest, AutoArgIndex) {
  EXPECT_EQ("abc", format("{}{}{}", 'a', 'b', 'c'));
  EXPECT_THROW_MSG(format("{0}{}", 'a', 'b'),
      FormatError, "cannot switch from manual to automatic argument indexing");
  EXPECT_THROW_MSG(format("{}{0}", 'a', 'b'),
      FormatError, "cannot switch from automatic to manual argument indexing");
  EXPECT_EQ("1.2", format("{:.{}}", 1.2345, 2));
  EXPECT_THROW_MSG(format("{0}:.{}", 1.2345, 2),
      FormatError, "cannot switch from manual to automatic argument indexing");
  EXPECT_THROW_MSG(format("{:.{0}}", 1.2345, 2),
      FormatError, "cannot switch from automatic to manual argument indexing");
  EXPECT_THROW_MSG(format("{}"), FormatError, "argument index out of range");
}