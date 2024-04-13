TEST_F(HTTP2CodecTest, BasicPushPromise) {
  upstreamCodec_.generateSettings(output_);
  parse();
  EXPECT_FALSE(upstreamCodec_.supportsPushTransactions());
  EXPECT_FALSE(downstreamCodec_.supportsPushTransactions());

  auto settings = upstreamCodec_.getEgressSettings();
  settings->setSetting(SettingsId::ENABLE_PUSH, 1);
  upstreamCodec_.generateSettings(output_);
  parse();
  EXPECT_TRUE(upstreamCodec_.supportsPushTransactions());
  EXPECT_TRUE(downstreamCodec_.supportsPushTransactions());

  SetUpUpstreamTest();

  HTTPCodec::StreamID assocStream = 7;
  for (auto i = 0; i < 2; i++) {
    // Push promise
    HTTPCodec::StreamID pushStream = downstreamCodec_.createStream();
    HTTPMessage req = getGetRequest();
    req.getHeaders().add(HTTP_HEADER_USER_AGENT, "coolio");
    downstreamCodec_.generatePushPromise(output_, pushStream, req, assocStream);

    parseUpstream();
    callbacks_.expectMessage(false, 2, "/"); // + host
    EXPECT_EQ(callbacks_.assocStreamId, assocStream);
    EXPECT_EQ(callbacks_.headersCompleteId, pushStream);
    auto& headers = callbacks_.msg->getHeaders();
    EXPECT_EQ("coolio", headers.getSingleOrEmpty(HTTP_HEADER_USER_AGENT));
    callbacks_.reset();

    // Actual reply headers
    HTTPMessage resp;
    resp.setStatusCode(200);
    resp.getHeaders().add(HTTP_HEADER_CONTENT_TYPE, "text/plain");
    downstreamCodec_.generateHeader(output_, pushStream, resp);

    parseUpstream();
    callbacks_.expectMessage(false, 2, 200);
    EXPECT_EQ(callbacks_.headersCompleteId, pushStream);
    EXPECT_EQ(callbacks_.assocStreamId, 0);
    EXPECT_TRUE(callbacks_.msg->getHeaders().exists(HTTP_HEADER_DATE));
    EXPECT_EQ("text/plain", callbacks_.msg->getHeaders().getSingleOrEmpty(
                  HTTP_HEADER_CONTENT_TYPE));
    callbacks_.reset();
  }
}