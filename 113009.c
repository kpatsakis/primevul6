TEST_F(HTTP2CodecTest, SettingsTableSize) {
  auto settings = upstreamCodec_.getEgressSettings();
  settings->setSetting(SettingsId::HEADER_TABLE_SIZE, 8192);
  upstreamCodec_.generateSettings(output_);

  parse();
  EXPECT_EQ(callbacks_.settings, 1);
  EXPECT_EQ(callbacks_.streamErrors, 0);
  EXPECT_EQ(callbacks_.sessionErrors, 0);
  downstreamCodec_.generateSettingsAck(output_);
  parseUpstream();

  callbacks_.reset();

  HTTPMessage resp;
  resp.setStatusCode(200);
  resp.setStatusMessage("nifty-nice");
  resp.getHeaders().add(HTTP_HEADER_CONTENT_TYPE, "x-coolio");
  SetUpUpstreamTest();
  downstreamCodec_.generateHeader(output_, 1, resp);

  parseUpstream();
  callbacks_.expectMessage(false, 2, 200);
  const auto& headers = callbacks_.msg->getHeaders();
  EXPECT_TRUE(callbacks_.msg->getHeaders().exists(HTTP_HEADER_DATE));
  EXPECT_EQ("x-coolio", headers.getSingleOrEmpty(HTTP_HEADER_CONTENT_TYPE));
}