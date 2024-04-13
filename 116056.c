TEST(WriterTest, WriteInt) {
  CHECK_WRITE(42);
  CHECK_WRITE(-42);
  CHECK_WRITE(static_cast<short>(12));
  CHECK_WRITE(34u);
  CHECK_WRITE(std::numeric_limits<int>::min());
  CHECK_WRITE(std::numeric_limits<int>::max());
  CHECK_WRITE(std::numeric_limits<unsigned>::max());
}