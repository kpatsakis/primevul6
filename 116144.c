TEST(ArrayWriterTest, Write) {
  char array[10];
  fmt::ArrayWriter w(array, sizeof(array));
  w.write("{}", 42);
  EXPECT_EQ("42", w.str());
}