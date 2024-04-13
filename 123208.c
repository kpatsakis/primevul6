TEST_F(PlaintextRecordTest, TestMaxSize) {
  addToQueue("1603014000");
  EXPECT_FALSE(read_.read(queue_).hasValue());
  EXPECT_EQ(queue_.chainLength(), 5);
}