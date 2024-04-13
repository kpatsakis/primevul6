TEST_F(PlaintextRecordTest, TestDataRemaining) {
  addToQueue("16030100050123456789160301");
  auto msg = read_.read(queue_);
  EXPECT_EQ(msg->type, ContentType::handshake);
  expectSame(msg->fragment, "0123456789");
  EXPECT_EQ(queue_.chainLength(), 3);
  expectSame(queue_.move(), "160301");
}