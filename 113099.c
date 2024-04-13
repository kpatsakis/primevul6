TEST_F(HTTP2CodecTest, BasicContinuation) {
  HTTPMessage req = getBigGetRequest();
  upstreamCodec_.generateHeader(output_, 1, req);

  parse();
  callbacks_.expectMessage(false, -1, "/");
#ifndef NDEBUG
  EXPECT_GT(downstreamCodec_.getReceivedFrameCount(), 1);
#endif
  const auto& headers = callbacks_.msg->getHeaders();
  EXPECT_EQ("coolio", headers.getSingleOrEmpty(HTTP_HEADER_USER_AGENT));
  EXPECT_EQ(callbacks_.messageBegin, 1);
  EXPECT_EQ(callbacks_.headersComplete, 1);
  EXPECT_EQ(callbacks_.messageComplete, 0);
  EXPECT_EQ(callbacks_.streamErrors, 0);
  EXPECT_EQ(callbacks_.sessionErrors, 0);
}