TEST_F(PlaintextRecordTest, TestWriteHandshake) {
  TLSMessage msg{ContentType::handshake, getBuf("1234567890")};
  auto buf = write_.write(std::move(msg));
  expectSame(buf.data, "16030300051234567890");
}