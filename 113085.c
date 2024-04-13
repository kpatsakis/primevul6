TEST_F(HTTP2CodecTest, BasicHeader) {
  HTTPMessage req = getGetRequest("/guacamole");
  req.getHeaders().add(HTTP_HEADER_USER_AGENT, "coolio");
  req.getHeaders().add("tab-hdr", "coolio\tv2");
  // Connection header will get dropped
  req.getHeaders().add(HTTP_HEADER_CONNECTION, "Love");
  req.setSecure(true);
  upstreamCodec_.generateHeader(output_, 1, req, true /* eom */);

  parse();
  callbacks_.expectMessage(true, 3, "/guacamole");
  EXPECT_TRUE(callbacks_.msg->isSecure());
  const auto& headers = callbacks_.msg->getHeaders();
  EXPECT_EQ("coolio", headers.getSingleOrEmpty(HTTP_HEADER_USER_AGENT));
  EXPECT_EQ("coolio\tv2", headers.getSingleOrEmpty("tab-hdr"));
  EXPECT_EQ("www.foo.com", headers.getSingleOrEmpty(HTTP_HEADER_HOST));
}