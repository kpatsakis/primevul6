TEST_F(PlaintextRecordTest, TestWriteClientHello) {
  auto buf = write_.writeInitialClientHello(getBuf("1234567890"));
  expectSame(buf.data, "16030100051234567890");
}