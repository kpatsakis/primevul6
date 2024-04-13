TEST_F(PlaintextRecordTest, TestOverSize) {
  addToQueue("1603014001");
  EXPECT_ANY_THROW(read_.read(queue_));
}