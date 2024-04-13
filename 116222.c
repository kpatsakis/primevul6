TEST(WriterTest, hexu) {
  using fmt::hexu;
  EXPECT_EQ("CAFE", (MemoryWriter() << hexu(0xcafe)).str());
  EXPECT_EQ("BABE", (MemoryWriter() << hexu(0xbabeu)).str());
  EXPECT_EQ("DEAD", (MemoryWriter() << hexu(0xdeadl)).str());
  EXPECT_EQ("BEEF", (MemoryWriter() << hexu(0xbeeful)).str());
  EXPECT_EQ("CAFEBABE", (MemoryWriter() << hexu(0xcafebabell)).str());
  EXPECT_EQ("DEADBEEF", (MemoryWriter() << hexu(0xdeadbeefull)).str());
}