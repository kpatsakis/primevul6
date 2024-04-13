TEST(ArrayWriterTest, BufferOverflow) {
  char array[10];
  fmt::ArrayWriter w(array, sizeof(array));
  w.write("{:10}", 1);
  EXPECT_THROW_MSG(w.write("{}", 1), std::runtime_error, "buffer overflow");
}