TEST_F(HTTP2CodecTest, SettingsTableSizeEarlyShrink) {
  // Lower size to 2k
  auto settings = upstreamCodec_.getEgressSettings();
  settings->setSetting(SettingsId::HEADER_TABLE_SIZE, 2048);
  upstreamCodec_.generateSettings(output_);

  parse();
  EXPECT_EQ(callbacks_.settings, 1);
  EXPECT_EQ(callbacks_.streamErrors, 0);
  EXPECT_EQ(callbacks_.sessionErrors, 0);
  downstreamCodec_.generateSettingsAck(output_);
  // Parsing SETTINGS ack updates upstream decoder to 2k
  parseUpstream();

  callbacks_.reset();

  HTTPMessage resp;
  resp.setStatusCode(200);
  resp.setStatusMessage("nifty-nice");
  resp.getHeaders().add(HTTP_HEADER_CONTENT_TYPE, "x-coolio");
  SetUpUpstreamTest();
  // downstream encoder will send TSU/2k
  downstreamCodec_.generateHeader(output_, 1, resp);

  // sets pending table size to 512, but doesn't update it yet
  settings = upstreamCodec_.getEgressSettings();
  settings->setSetting(SettingsId::HEADER_TABLE_SIZE, 512);
  IOBufQueue tmp{IOBufQueue::cacheChainLength()};
  upstreamCodec_.generateSettings(tmp);

  // Previous code would barf here, since TSU/2k is a violation of the current
  // max=512
  parseUpstream();
  callbacks_.expectMessage(false, 2, 200);
  const auto& headers = callbacks_.msg->getHeaders();
  EXPECT_TRUE(callbacks_.msg->getHeaders().exists(HTTP_HEADER_DATE));
  EXPECT_EQ("x-coolio", headers.getSingleOrEmpty(HTTP_HEADER_CONTENT_TYPE));
}