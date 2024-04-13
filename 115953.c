TEST(WriterTest, Ctor) {
  MemoryWriter w;
  EXPECT_EQ(0u, w.size());
  EXPECT_STREQ("", w.c_str());
  EXPECT_EQ("", w.str());
}