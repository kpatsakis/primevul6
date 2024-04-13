TEST(ProtocolSkipTest, SkipStopInContainer) {
  IOBufQueue queue;
  CompactProtocolWriter writer;
  writer.setOutput(&queue);
  writer.writeListBegin(TType::T_STOP, 1u << 30);
  auto buf = queue.move();
  CompactProtocolReader reader;
  reader.setInput(buf.get());
  bool thrown = false;
  try {
    reader.skip(TType::T_LIST);
  } catch (const TProtocolException& ex) {
    EXPECT_EQ(TProtocolException::INVALID_DATA, ex.getType());
    thrown = true;
  }
  EXPECT_TRUE(thrown);
}