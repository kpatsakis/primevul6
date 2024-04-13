TEST_F(PlaintextRecordTest, TestEmpty) {
  addToQueue("1603010000aa");
  EXPECT_ANY_THROW(read_.read(queue_));
}