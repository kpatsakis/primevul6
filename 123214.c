TEST_F(PlaintextRecordTest, TestFragmentedWrite) {
  TLSMessage msg{ContentType::handshake, IOBuf::create(0)};
  auto buf = IOBuf::create(0x4010);
  buf->append(0x4010);
  memset(buf->writableData(), 0x1, buf->length());
  msg.fragment->prependChain(std::move(buf));
  auto write = write_.write(std::move(msg));

  TLSMessage msg1{ContentType::handshake, IOBuf::create(0)};
  buf = IOBuf::create(0x4000);
  buf->append(0x4000);
  memset(buf->writableData(), 0x1, buf->length());
  msg1.fragment->prependChain(std::move(buf));
  auto write1 = write_.write(std::move(msg1));

  TLSMessage msg2{ContentType::handshake, IOBuf::create(0)};
  buf = IOBuf::create(0x10);
  buf->append(0x10);
  memset(buf->writableData(), 0x1, buf->length());
  msg2.fragment->prependChain(std::move(buf));
  auto write2 = write_.write(std::move(msg2));

  write1.data->prependChain(std::move(write2.data));
  IOBufEqualTo eq;
  EXPECT_TRUE(eq(write.data, write1.data));
}