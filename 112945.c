TEST_F(HTTP2CodecTest, ResponseFromClient) {
  // this is to test EX_HEADERS frame, which carrys the HTTP response replied by
  // client side
  downstreamCodec_.getEgressSettings()->setSetting(
      SettingsId::ENABLE_EX_HEADERS, 1);
  proxygen::http2::writeSettings(
      output_, {{proxygen::SettingsId::ENABLE_EX_HEADERS, 1}});

  HTTPMessage resp;
  resp.setStatusCode(200);
  resp.setStatusMessage("nifty-nice");
  resp.getHeaders().add(HTTP_HEADER_CONTENT_TYPE, "x-coolio");

  HTTPCodec::StreamID stream = folly::Random::rand32(10, 1024) * 2;
  HTTPCodec::StreamID controlStream = folly::Random::rand32(10, 1024) * 2 + 1;
  downstreamCodec_.generateExHeader(output_, stream, resp,
    HTTPCodec::ExAttributes(controlStream, true), true);

  parse();
  EXPECT_EQ(controlStream, callbacks_.controlStreamId);
  EXPECT_TRUE(callbacks_.isUnidirectional);
  EXPECT_EQ("OK", callbacks_.msg->getStatusMessage());
  callbacks_.expectMessage(true, 2, 200);
  const auto& headers = callbacks_.msg->getHeaders();
  EXPECT_EQ("OK", callbacks_.msg->getStatusMessage());
  EXPECT_TRUE(callbacks_.msg->getHeaders().exists(HTTP_HEADER_DATE));
  EXPECT_EQ("x-coolio", headers.getSingleOrEmpty(HTTP_HEADER_CONTENT_TYPE));
}