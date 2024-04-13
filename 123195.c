TEST_F(PlaintextRecordTest, TestSkipAndRead) {
  read_.setSkipEncryptedRecords(true);
  addToQueue("170301000501234567891703010005012345678916030100050123456789");
  auto msg = read_.read(queue_);
  EXPECT_EQ(msg->type, ContentType::handshake);
  expectSame(msg->fragment, "0123456789");
  EXPECT_TRUE(queue_.empty());
}