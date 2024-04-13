TEST_F(PlaintextRecordTest, TestSkipOversizedRecord) {
  read_.setSkipEncryptedRecords(true);
  addToQueue("170301fffb");
  auto longBuf = IOBuf::create(0xfffb);
  longBuf->append(0xfffb);
  queue_.append(std::move(longBuf));
  EXPECT_FALSE(read_.read(queue_).hasValue());
  EXPECT_TRUE(queue_.empty());
}