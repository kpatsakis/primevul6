TEST_F(HTTP2CodecTest, BasicPing) {
  upstreamCodec_.generatePingRequest(output_);
  upstreamCodec_.generatePingReply(output_, 17);

  uint64_t pingReq;
  parse([&] (IOBuf* ingress) {
      folly::io::Cursor c(ingress);
      c.skip(http2::kFrameHeaderSize + http2::kConnectionPreface.length());
      pingReq = c.read<uint64_t>();
    });

  EXPECT_EQ(callbacks_.messageBegin, 0);
  EXPECT_EQ(callbacks_.headersComplete, 0);
  EXPECT_EQ(callbacks_.messageComplete, 0);
  EXPECT_EQ(callbacks_.bodyCalls, 0);
  EXPECT_EQ(callbacks_.recvPingRequest, pingReq);
  EXPECT_EQ(callbacks_.recvPingReply, 17);
  EXPECT_EQ(callbacks_.streamErrors, 0);
  EXPECT_EQ(callbacks_.sessionErrors, 0);
}