TEST(WriterTest, Data) {
  MemoryWriter w;
  w << 42;
  EXPECT_EQ("42", std::string(w.data(), w.size()));
}