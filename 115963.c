TEST(WriterTest, WriteDoubleAtBufferBoundary) {
  MemoryWriter writer;
  for (int i = 0; i < 100; ++i)
    writer << 1.23456789;
}