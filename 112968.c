TEST_F(HTTP2CodecTest, DuplicateBadHeaderPriority) {
  // Sent an initial header with a circular dependency
  HTTPMessage req = getGetRequest();
  req.setHTTP2Priority(HTTPMessage::HTTPPriority(0, false, 7));
  upstreamCodec_.generateHeader(output_, 1, req, true /* eom */);

  // Hack ingress with circular dependency.
  EXPECT_TRUE(parse([&](IOBuf* ingress) {
    folly::io::RWPrivateCursor c(ingress);
    c.skip(http2::kFrameHeaderSize + http2::kConnectionPreface.length());
    c.writeBE<uint32_t>(1);
  }));

  EXPECT_EQ(callbacks_.streamErrors, 1);
  EXPECT_EQ(callbacks_.sessionErrors, 0);

  // On the same stream, send another request.
  HTTPMessage nextRequest = getGetRequest();
  upstreamCodec_.generateHeader(output_, 1, nextRequest, true /* eom */);
  parse();
  EXPECT_EQ(callbacks_.streamErrors, 2);
  EXPECT_EQ(callbacks_.sessionErrors, 0);
}