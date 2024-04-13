TEST(FormatterTest, Examples) {
  EXPECT_EQ("First, thou shalt count to three",
      format("First, thou shalt count to {0}", "three"));
  EXPECT_EQ("Bring me a shrubbery",
      format("Bring me a {}", "shrubbery"));
  EXPECT_EQ("From 1 to 3", format("From {} to {}", 1, 3));

  char buffer[BUFFER_SIZE];
  safe_sprintf(buffer, "%03.2f", -1.2);
  EXPECT_EQ(buffer, format("{:03.2f}", -1.2));

  EXPECT_EQ("a, b, c", format("{0}, {1}, {2}", 'a', 'b', 'c'));
  EXPECT_EQ("a, b, c", format("{}, {}, {}", 'a', 'b', 'c'));
  EXPECT_EQ("c, b, a", format("{2}, {1}, {0}", 'a', 'b', 'c'));
  EXPECT_EQ("abracadabra", format("{0}{1}{0}", "abra", "cad"));

  EXPECT_EQ("left aligned                  ",
      format("{:<30}", "left aligned"));
  EXPECT_EQ("                 right aligned",
      format("{:>30}", "right aligned"));
  EXPECT_EQ("           centered           ",
      format("{:^30}", "centered"));
  EXPECT_EQ("***********centered***********",
      format("{:*^30}", "centered"));

  EXPECT_EQ("+3.140000; -3.140000",
      format("{:+f}; {:+f}", 3.14, -3.14));
  EXPECT_EQ(" 3.140000; -3.140000",
      format("{: f}; {: f}", 3.14, -3.14));
  EXPECT_EQ("3.140000; -3.140000",
      format("{:-f}; {:-f}", 3.14, -3.14));

  EXPECT_EQ("int: 42;  hex: 2a;  oct: 52",
      format("int: {0:d};  hex: {0:x};  oct: {0:o}", 42));
  EXPECT_EQ("int: 42;  hex: 0x2a;  oct: 052",
      format("int: {0:d};  hex: {0:#x};  oct: {0:#o}", 42));

  EXPECT_EQ("The answer is 42", format("The answer is {}", 42));
  EXPECT_THROW_MSG(
    format("The answer is {:d}", "forty-two"), FormatError,
    "unknown format code 'd' for string");

  EXPECT_EQ(L"Cyrillic letter \x42e",
    format(L"Cyrillic letter {}", L'\x42e'));

  EXPECT_WRITE(stdout,
      fmt::print("{}", std::numeric_limits<double>::infinity()), "inf");
}