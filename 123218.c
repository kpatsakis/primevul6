TEST_F(PlaintextRecordTest, TestSkipAndWait) {
  read_.setSkipEncryptedRecords(true);
  addToQueue("17030100050123456789");
  EXPECT_FALSE(read_.read(queue_).hasValue());
  EXPECT_TRUE(queue_.empty());
}