TEST_F(HTTP2CodecTest, BasicHeaderReply) {
  SetUpUpstreamTest();
  HTTPMessage resp;
  resp.setStatusCode(200);
  resp.setStatusMessage("nifty-nice");
  resp.getHeaders().add(HTTP_HEADER_CONTENT_TYPE, "x-coolio");
  downstreamCodec_.generateHeader(output_, 1, resp);
  downstreamCodec_.generateEOM(output_, 1);

  parseUpstream();
  callbacks_.expectMessage(true, 2, 200);
  const auto& headers = callbacks_.msg->getHeaders();
  // HTTP/2 doesnt support serialization - instead you get the default
  EXPECT_EQ("OK", callbacks_.msg->getStatusMessage());
  EXPECT_TRUE(callbacks_.msg->getHeaders().exists(HTTP_HEADER_DATE));
  EXPECT_EQ("x-coolio", headers.getSingleOrEmpty(HTTP_HEADER_CONTENT_TYPE));
}