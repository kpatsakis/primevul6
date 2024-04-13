TEST(WriterTest, NoConflictWithIOManip) {
  using namespace std;
  using namespace fmt;
  EXPECT_EQ("cafe", (MemoryWriter() << hex(0xcafe)).str());
  EXPECT_EQ("12", (MemoryWriter() << oct(012)).str());
}