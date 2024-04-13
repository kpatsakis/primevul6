TEST_F(PlaintextRecordTest, TestReadEmpty) {
  EXPECT_FALSE(read_.read(queue_).hasValue());
}