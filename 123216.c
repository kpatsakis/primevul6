TEST_F(PlaintextRecordTest, TestWaitForData) {
  addToQueue("160301000512345678");
  EXPECT_FALSE(read_.read(queue_).hasValue());
  EXPECT_EQ(queue_.chainLength(), 9);
}