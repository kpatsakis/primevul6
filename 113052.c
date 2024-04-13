TEST_F(HTTP2CodecTest, MissingContinuation) {
  IOBufQueue output(IOBufQueue::cacheChainLength());
  HTTPMessage req = getBigGetRequest();

  upstreamCodec_.generateHeader(output_, 1, req);
  // empirically determined the size of continuation frame, and strip it
  output_.trimEnd(http2::kFrameHeaderSize + 4134);

  // insert a non-continuation (but otherwise valid) frame
  http2::writeGoaway(output_, 17, ErrorCode::ENHANCE_YOUR_CALM);

  parse();
  EXPECT_EQ(callbacks_.messageBegin, 0);
  EXPECT_EQ(callbacks_.headersComplete, 0);
  EXPECT_EQ(callbacks_.messageComplete, 0);
  EXPECT_EQ(callbacks_.streamErrors, 0);
  EXPECT_EQ(callbacks_.sessionErrors, 1);
#ifndef NDEBUG
  EXPECT_EQ(downstreamCodec_.getReceivedFrameCount(), 2);
#endif
}