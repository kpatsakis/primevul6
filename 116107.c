TEST(ArrayWriterTest, WChar) {
  wchar_t array[10];
  fmt::WArrayWriter w(array);
  w.write(L"{}", 42);
  EXPECT_EQ(L"42", w.str());
}