TEST(WriterTest, PadString) {
  EXPECT_EQ("test    ", (MemoryWriter() << pad("test", 8)).str());
  EXPECT_EQ("test******", (MemoryWriter() << pad("test", 10, '*')).str());
}