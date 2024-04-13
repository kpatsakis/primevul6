TEST(FormatterTest, FormatCString) {
  check_unknown_types("test", "sp", "string");
  EXPECT_EQ("test", format("{0}", "test"));
  EXPECT_EQ("test", format("{0:s}", "test"));
  char nonconst[] = "nonconst";
  EXPECT_EQ("nonconst", format("{0}", nonconst));
  EXPECT_THROW_MSG(format("{0}", reinterpret_cast<const char*>(0)),
      FormatError, "string pointer is null");
}