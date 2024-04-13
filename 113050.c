TEST_F(HTTP2CodecTest, BasicContinuationEndStream) {
  // CONTINUATION with END_STREAM flag set on the preceding HEADERS frame
  HTTPMessage req = getBigGetRequest();
  upstreamCodec_.generateHeader(output_, 1, req, true /* eom */);

  parse();
  callbacks_.expectMessage(true, -1, "/");
#ifndef NDEBUG
  EXPECT_GT(downstreamCodec_.getReceivedFrameCount(), 1);
#endif
  const auto& headers = callbacks_.msg->getHeaders();
  EXPECT_EQ("coolio", headers.getSingleOrEmpty(HTTP_HEADER_USER_AGENT));
  EXPECT_EQ(callbacks_.messageBegin, 1);
  EXPECT_EQ(callbacks_.headersComplete, 1);
  EXPECT_EQ(callbacks_.messageComplete, 1);
  EXPECT_EQ(callbacks_.streamErrors, 0);
  EXPECT_EQ(callbacks_.sessionErrors, 0);
}