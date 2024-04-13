TEST_F(PlaintextRecordTest, TestWaitForHeader) {
  addToQueue("16030102");
  EXPECT_FALSE(read_.read(queue_).hasValue());
  EXPECT_EQ(queue_.chainLength(), 4);
}