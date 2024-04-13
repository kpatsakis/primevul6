TEST(WriterTest, oct) {
  using fmt::oct;
  EXPECT_EQ("12", (MemoryWriter() << oct(static_cast<short>(012))).str());
  EXPECT_EQ("12", (MemoryWriter() << oct(012)).str());
  EXPECT_EQ("34", (MemoryWriter() << oct(034u)).str());
  EXPECT_EQ("56", (MemoryWriter() << oct(056l)).str());
  EXPECT_EQ("70", (MemoryWriter() << oct(070ul)).str());
  EXPECT_EQ("1234", (MemoryWriter() << oct(01234ll)).str());
  EXPECT_EQ("5670", (MemoryWriter() << oct(05670ull)).str());
}