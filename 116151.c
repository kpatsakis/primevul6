TEST(WriterTest, hex) {
  using fmt::hex;
  fmt::IntFormatSpec<int, fmt::TypeSpec<'x'> > (*phex)(int value) = hex;
  phex(42);
  // This shouldn't compile:
  //fmt::IntFormatSpec<short, fmt::TypeSpec<'x'> > (*phex2)(short value) = hex;

  EXPECT_EQ("cafe", (MemoryWriter() << hex(0xcafe)).str());
  EXPECT_EQ("babe", (MemoryWriter() << hex(0xbabeu)).str());
  EXPECT_EQ("dead", (MemoryWriter() << hex(0xdeadl)).str());
  EXPECT_EQ("beef", (MemoryWriter() << hex(0xbeeful)).str());
  EXPECT_EQ("cafebabe", (MemoryWriter() << hex(0xcafebabell)).str());
  EXPECT_EQ("deadbeef", (MemoryWriter() << hex(0xdeadbeefull)).str());
}