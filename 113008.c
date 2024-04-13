TEST_F(HTTP2CodecTest, BasicRstInvalidCode) {
  upstreamCodec_.generateRstStream(output_, 2, ErrorCode::_SPDY_INVALID_STREAM);
  parse();
  EXPECT_EQ(callbacks_.messageBegin, 0);
  EXPECT_EQ(callbacks_.headersComplete, 0);
  EXPECT_EQ(callbacks_.messageComplete, 0);
  EXPECT_EQ(callbacks_.bodyCalls, 0);
  EXPECT_EQ(callbacks_.aborts, 1);
  EXPECT_EQ(callbacks_.streamErrors, 0);
  EXPECT_EQ(callbacks_.sessionErrors, 0);
}