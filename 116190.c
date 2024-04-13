TEST(WriterTest, Format) {
  MemoryWriter w;
  w.write("part{0}", 1);
  EXPECT_EQ(strlen("part1"), w.size());
  EXPECT_STREQ("part1", w.c_str());
  EXPECT_STREQ("part1", w.data());
  EXPECT_EQ("part1", w.str());
  w.write("part{0}", 2);
  EXPECT_EQ(strlen("part1part2"), w.size());
  EXPECT_STREQ("part1part2", w.c_str());
  EXPECT_STREQ("part1part2", w.data());
  EXPECT_EQ("part1part2", w.str());
}