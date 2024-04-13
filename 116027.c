TEST(WriterTest, WriteString) {
  CHECK_WRITE_CHAR("abc");
  CHECK_WRITE_WCHAR("abc");
  // The following line shouldn't compile:
  //MemoryWriter() << L"abc";
}