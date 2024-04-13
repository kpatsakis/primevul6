TEST_F(HTTP2CodecTest, ZeroWindow) {
  auto streamID = HTTPCodec::StreamID(1);
  // First generate a frame with delta=1 so as to pass the checks, and then
  // hack the frame so that delta=0 without modifying other checks
  upstreamCodec_.generateWindowUpdate(output_, streamID, 1);
  output_.trimEnd(http2::kFrameWindowUpdateSize);
  QueueAppender appender(&output_, http2::kFrameWindowUpdateSize);
  appender.writeBE<uint32_t>(0);

  parse();
  // This test doesn't ensure that RST_STREAM is generated
  EXPECT_EQ(callbacks_.windowUpdateCalls, 0);
  EXPECT_EQ(callbacks_.streamErrors, 1);
  EXPECT_EQ(callbacks_.lastParseError->getCodecStatusCode(),
      ErrorCode::PROTOCOL_ERROR);
}