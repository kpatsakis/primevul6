TEST_F(PlaintextRecordTest, TestReadAppData) {
  addToQueue("17030100050123456789");
  EXPECT_ANY_THROW(read_.read(queue_));
}