TEST(ArrayWriterTest, Ctor) {
  char array[10] = "garbage";
  fmt::ArrayWriter w(array, sizeof(array));
  EXPECT_EQ(0u, w.size());
  EXPECT_STREQ("", w.c_str());
}