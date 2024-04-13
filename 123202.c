TEST_F(PlaintextRecordTest, TestWriteAppData) {
  TLSMessage msg{ContentType::application_data};
  EXPECT_ANY_THROW(write_.write(std::move(msg)));
}