TEST_F(PlaintextRecordTest, TestReadAlert) {
  addToQueue("15030100050123456789");
  auto msg = read_.read(queue_);
  EXPECT_EQ(msg->type, ContentType::alert);
  expectSame(msg->fragment, "0123456789");
  EXPECT_TRUE(queue_.empty());
}