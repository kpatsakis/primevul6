TEST_F(HTTP2CodecTest, BadPushPromise) {
  // ENABLE_PUSH is now 0 by default
  SetUpUpstreamTest();
  HTTPMessage req = getGetRequest();
  req.getHeaders().add(HTTP_HEADER_USER_AGENT, "coolio");
  downstreamCodec_.generatePushPromise(output_, 2, req, 1);

  parseUpstream();
  EXPECT_EQ(callbacks_.messageBegin, 0);
  EXPECT_EQ(callbacks_.headersComplete, 0);
  EXPECT_EQ(callbacks_.messageComplete, 0);
  EXPECT_EQ(callbacks_.assocStreamId, 0);
  EXPECT_EQ(callbacks_.streamErrors, 0);
  EXPECT_EQ(callbacks_.sessionErrors, 1);
}