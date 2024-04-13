TEST(ProtocolSkipTest, SkipStop) {
  IOBufQueue queue;
  CompactProtocolWriter writer;
  writer.setOutput(&queue);
  writer.writeFieldStop();
  auto buf = queue.move();
  CompactProtocolReader reader;
  reader.setInput(buf.get());
  bool thrown = false;
  try {
    reader.skip(TType::T_STOP);
  } catch (const TProtocolException& ex) {
    EXPECT_EQ(TProtocolException::INVALID_DATA, ex.getType());
    thrown = true;
  }
  EXPECT_TRUE(thrown);
}