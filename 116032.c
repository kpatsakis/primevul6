TEST(WriterTest, WriteDouble) {
  CHECK_WRITE(4.2);
  CHECK_WRITE(-4.2);
  CHECK_WRITE(std::numeric_limits<double>::min());
  CHECK_WRITE(std::numeric_limits<double>::max());
}