TEST_F(PlaintextRecordTest, TestWaitBeforeSkip) {
  read_.setSkipEncryptedRecords(true);
  addToQueue("170301000501234567");
  EXPECT_FALSE(read_.read(queue_).hasValue());
  expectSame(queue_.move(), "170301000501234567");
}