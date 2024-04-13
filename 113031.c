TEST_F(HTTP2CodecTest, BadHeaderPriority) {
  HTTPMessage req = getGetRequest();
  req.setHTTP2Priority(HTTPMessage::HTTPPriority(0, false, 7));
  upstreamCodec_.generateHeader(output_, 1, req, true /* eom */);

  // hack ingress with cirular dep
  EXPECT_TRUE(parse([&] (IOBuf* ingress) {
        folly::io::RWPrivateCursor c(ingress);
        c.skip(http2::kFrameHeaderSize + http2::kConnectionPreface.length());
        c.writeBE<uint32_t>(1);
      }));

  EXPECT_EQ(callbacks_.streamErrors, 1);
  EXPECT_EQ(callbacks_.sessionErrors, 0);
}