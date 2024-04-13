TEST(ArrayWriterTest, CompileTimeSizeCtor) {
  char array[10] = "garbage";
  fmt::ArrayWriter w(array);
  EXPECT_EQ(0u, w.size());
  EXPECT_STREQ("", w.c_str());
  w.write("{:10}", 1);
}