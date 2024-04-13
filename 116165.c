TEST(FormatterTest, FormatExamples) {
  using fmt::hex;
  EXPECT_EQ("0000cafe", (MemoryWriter() << pad(hex(0xcafe), 8, '0')).str());

  std::string message = format("The answer is {}", 42);
  EXPECT_EQ("The answer is 42", message);

  EXPECT_EQ("42", format("{}", 42));
  EXPECT_EQ("42", format(std::string("{}"), 42));

  MemoryWriter out;
  out << "The answer is " << 42 << "\n";
  out.write("({:+f}, {:+f})", -3.14, 3.14);
  EXPECT_EQ("The answer is 42\n(-3.140000, +3.140000)", out.str());

  {
    MemoryWriter writer;
    for (int i = 0; i < 10; i++)
      writer.write("{}", i);
    std::string s = writer.str(); // s == 0123456789
    EXPECT_EQ("0123456789", s);
  }

  const char *filename = "nonexistent";
  FILE *ftest = safe_fopen(filename, "r");
  if (ftest) fclose(ftest);
  int error_code = errno;
  EXPECT_TRUE(ftest == 0);
  EXPECT_SYSTEM_ERROR({
    FILE *f = safe_fopen(filename, "r");
    if (!f)
      throw fmt::SystemError(errno, "Cannot open file '{}'", filename);
    fclose(f);
  }, error_code, "Cannot open file 'nonexistent'");
}