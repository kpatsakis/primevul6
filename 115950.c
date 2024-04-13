TEST(WriterTest, PadWString) {
  EXPECT_EQ(L"test    ", (WMemoryWriter() << pad(L"test", 8)).str());
  EXPECT_EQ(L"test******", (WMemoryWriter() << pad(L"test", 10, '*')).str());
  EXPECT_EQ(L"test******", (WMemoryWriter() << pad(L"test", 10, L'*')).str());
}