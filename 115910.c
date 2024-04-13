TEST(WriterTest, WriteWithoutArgs) {
  MemoryWriter w;
  w.write("test");
  EXPECT_EQ("test", std::string(w.data(), w.size()));
}