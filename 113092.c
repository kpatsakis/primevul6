TEST_F(HTTP2CodecTest, RequestFromServer) {
  // this is to test EX_HEADERS frame, which carrys the HTTP request initiated
  // by server side
  upstreamCodec_.getEgressSettings()->setSetting(
      SettingsId::ENABLE_EX_HEADERS, 1);
  SetUpUpstreamTest();
  proxygen::http2::writeSettings(
      output_, {{proxygen::SettingsId::ENABLE_EX_HEADERS, 1}});

  HTTPMessage req = getGetRequest("/guacamole");
  req.getHeaders().add(HTTP_HEADER_USER_AGENT, "coolio");
  req.getHeaders().add("tab-hdr", "coolio\tv2");
  // Connection header will get dropped
  req.getHeaders().add(HTTP_HEADER_CONNECTION, "Love");
  req.setSecure(true);

  HTTPCodec::StreamID stream = folly::Random::rand32(10, 1024) * 2;
  HTTPCodec::StreamID controlStream = folly::Random::rand32(10, 1024) * 2 + 1;
  upstreamCodec_.generateExHeader(output_, stream, req,
                                  HTTPCodec::ExAttributes(controlStream, true),
                                  true);

  parseUpstream();
  EXPECT_EQ(controlStream, callbacks_.controlStreamId);
  EXPECT_TRUE(callbacks_.isUnidirectional);
  callbacks_.expectMessage(true, 3, "/guacamole");
  EXPECT_TRUE(callbacks_.msg->isSecure());
  const auto& headers = callbacks_.msg->getHeaders();
  EXPECT_EQ("coolio", headers.getSingleOrEmpty(HTTP_HEADER_USER_AGENT));
  EXPECT_EQ("coolio\tv2", headers.getSingleOrEmpty("tab-hdr"));
  EXPECT_EQ("www.foo.com", headers.getSingleOrEmpty(HTTP_HEADER_HOST));
}