TEST_F(HTTP2CodecTest, BigHeaderCompressed) {
  SetUpUpstreamTest();
  auto settings = downstreamCodec_.getEgressSettings();
  settings->setSetting(SettingsId::MAX_HEADER_LIST_SIZE, 37);
  downstreamCodec_.generateSettings(output_);
  parseUpstream();

  SetUp();
  HTTPMessage req = getGetRequest("/guacamole");
  req.getHeaders().add(HTTP_HEADER_USER_AGENT, "coolio");
  upstreamCodec_.generateHeader(output_, 1, req, true /* eom */);

  parse();
  // session error
  EXPECT_EQ(callbacks_.messageBegin, 0);
  EXPECT_EQ(callbacks_.headersComplete, 0);
  EXPECT_EQ(callbacks_.messageComplete, 0);
  EXPECT_EQ(callbacks_.streamErrors, 0);
  EXPECT_EQ(callbacks_.sessionErrors, 1);
}