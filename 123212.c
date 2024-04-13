TEST_F(PlaintextRecordTest, TestReadHandshake) {
  addToQueue("16030100050123456789");
  auto msg = read_.read(queue_);
  EXPECT_EQ(msg->type, ContentType::handshake);
  expectSame(msg->fragment, "0123456789");
  EXPECT_TRUE(queue_.empty());
}