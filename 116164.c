TEST(WriterTest, WriteLong) {
  CHECK_WRITE(56l);
  CHECK_WRITE(78ul);
  CHECK_WRITE(std::numeric_limits<long>::min());
  CHECK_WRITE(std::numeric_limits<long>::max());
  CHECK_WRITE(std::numeric_limits<unsigned long>::max());
}