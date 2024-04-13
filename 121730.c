TEST(ProtocolSkipTest, SkipInt) {
  IOBufQueue queue;
  CompactProtocolWriter writer;
  writer.setOutput(&queue);
  writer.writeI32(123);
  auto buf = queue.move();
  CompactProtocolReader reader;
  reader.setInput(buf.get());
  reader.skip(TType::T_I32);
}