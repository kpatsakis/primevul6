TEST_F(HTTP2CodecTest, BadContinuationStream) {
  HTTPMessage req = getBigGetRequest();
  upstreamCodec_.generateHeader(output_, 1, req);

  // empirically determined the size of continuation frame, and fake it
  output_.trimEnd(http2::kFrameHeaderSize + 4134);
  auto fakeHeaders = makeBuf(4134);
  http2::writeContinuation(output_, 3, true, std::move(fakeHeaders));

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