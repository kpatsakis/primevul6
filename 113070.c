TEST_F(HTTP2CodecTest, BadPriority) {
  auto pri = HTTPMessage::HTTPPriority(0, true, 1);
  upstreamCodec_.generatePriority(output_, 1, pri);

  // hack ingress with cirular dep
  EXPECT_TRUE(parse([&] (IOBuf* ingress) {
        folly::io::RWPrivateCursor c(ingress);
        c.skip(http2::kFrameHeaderSize + http2::kConnectionPreface.length());
        c.writeBE<uint32_t>(1);
      }));

  EXPECT_EQ(callbacks_.streamErrors, 1);
  EXPECT_EQ(callbacks_.sessionErrors, 0);
}