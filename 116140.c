TEST(WriterTest, WWriter) {
  EXPECT_EQ(L"cafe", (fmt::WMemoryWriter() << fmt::hex(0xcafe)).str());
}