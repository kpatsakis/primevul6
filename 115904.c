TEST(WriterTest, pad) {
  using fmt::hex;
  EXPECT_EQ("    cafe", (MemoryWriter() << pad(hex(0xcafe), 8)).str());
  EXPECT_EQ("    babe", (MemoryWriter() << pad(hex(0xbabeu), 8)).str());
  EXPECT_EQ("    dead", (MemoryWriter() << pad(hex(0xdeadl), 8)).str());
  EXPECT_EQ("    beef", (MemoryWriter() << pad(hex(0xbeeful), 8)).str());
  EXPECT_EQ("    dead", (MemoryWriter() << pad(hex(0xdeadll), 8)).str());
  EXPECT_EQ("    beef", (MemoryWriter() << pad(hex(0xbeefull), 8)).str());

  EXPECT_EQ("     11", (MemoryWriter() << pad(11, 7)).str());
  EXPECT_EQ("     22", (MemoryWriter() << pad(22u, 7)).str());
  EXPECT_EQ("     33", (MemoryWriter() << pad(33l, 7)).str());
  EXPECT_EQ("     44", (MemoryWriter() << pad(44ul, 7)).str());
  EXPECT_EQ("     33", (MemoryWriter() << pad(33ll, 7)).str());
  EXPECT_EQ("     44", (MemoryWriter() << pad(44ull, 7)).str());

  MemoryWriter w;
  w.clear();
  w << pad(42, 5, '0');
  EXPECT_EQ("00042", w.str());
  w.clear();
  w << Date(2012, 12, 9);
  EXPECT_EQ("2012-12-9", w.str());
  w.clear();
  w << iso8601(Date(2012, 1, 9));
  EXPECT_EQ("2012-01-09", w.str());
}