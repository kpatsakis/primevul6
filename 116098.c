TEST(WriterTest, WriteLongLong) {
  CHECK_WRITE(56ll);
  CHECK_WRITE(78ull);
  CHECK_WRITE(std::numeric_limits<long long>::min());
  CHECK_WRITE(std::numeric_limits<long long>::max());
  CHECK_WRITE(std::numeric_limits<unsigned long long>::max());
}