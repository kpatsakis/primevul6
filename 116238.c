TEST(WriterTest, WriteDoubleWithFilledBuffer) {
  MemoryWriter writer;
  // Fill the buffer.
  for (int i = 0; i < fmt::internal::INLINE_BUFFER_SIZE; ++i)
    writer << ' ';
  writer << 1.2;
  EXPECT_STREQ("1.2", writer.c_str() + fmt::internal::INLINE_BUFFER_SIZE);
}