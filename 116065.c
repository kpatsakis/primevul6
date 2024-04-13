TEST(FormatterTest, FormatPointer) {
  check_unknown_types(reinterpret_cast<void*>(0x1234), "p", "pointer");
  EXPECT_EQ("0x0", format("{0}", reinterpret_cast<void*>(0)));
  EXPECT_EQ("0x1234", format("{0}", reinterpret_cast<void*>(0x1234)));
  EXPECT_EQ("0x1234", format("{0:p}", reinterpret_cast<void*>(0x1234)));
  EXPECT_EQ("0x" + std::string(sizeof(void*) * CHAR_BIT / 4, 'f'),
      format("{0}", reinterpret_cast<void*>(~uintptr_t())));
}