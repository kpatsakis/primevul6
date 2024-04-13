void HTTP2CodecTest::testHeaderListSize(bool oversized) {
  if (oversized) {
    auto settings = downstreamCodec_.getEgressSettings();
    settings->setSetting(SettingsId::MAX_HEADER_LIST_SIZE, 37);
  }

  HTTPMessage req = getGetRequest("/guacamole");
  req.getHeaders().add(HTTP_HEADER_USER_AGENT, "coolio");
  req.getHeaders().add("x-long-long-header",
                       "supercalafragalisticexpialadoshus");
  upstreamCodec_.generateHeader(output_, 1, req, true /* eom */);

  parse();
  // session error
  EXPECT_EQ(callbacks_.messageBegin, oversized ? 0 : 1);
  EXPECT_EQ(callbacks_.headersComplete, oversized ? 0 : 1);
  EXPECT_EQ(callbacks_.messageComplete, oversized ? 0 : 1);
  EXPECT_EQ(callbacks_.streamErrors, 0);
  EXPECT_EQ(callbacks_.sessionErrors, oversized ? 1 : 0);
}