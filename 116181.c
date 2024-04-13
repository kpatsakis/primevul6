TEST(WriterTest, bin) {
  using fmt::bin;
  EXPECT_EQ("1100101011111110", (MemoryWriter() << bin(0xcafe)).str());
  EXPECT_EQ("1011101010111110", (MemoryWriter() << bin(0xbabeu)).str());
  EXPECT_EQ("1101111010101101", (MemoryWriter() << bin(0xdeadl)).str());
  EXPECT_EQ("1011111011101111", (MemoryWriter() << bin(0xbeeful)).str());
  EXPECT_EQ("11001010111111101011101010111110",
            (MemoryWriter() << bin(0xcafebabell)).str());
  EXPECT_EQ("11011110101011011011111011101111",
            (MemoryWriter() << bin(0xdeadbeefull)).str());
}