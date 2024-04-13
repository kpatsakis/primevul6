TEST(WriterTest, WriteWideString) {
  CHECK_WRITE_WCHAR(L"abc");
  // The following line shouldn't compile:
  //fmt::WMemoryWriter() << "abc";
}