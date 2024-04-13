void HTTP2CodecTest::testFrameSizeLimit(bool oversized) {
  HTTPMessage req = getBigGetRequest("/guacamole");
  auto settings = downstreamCodec_.getEgressSettings();

  parse(); // consume preface
  if (oversized) {
    // trick upstream for sending a 2x bigger HEADERS frame
    settings->setSetting(SettingsId::MAX_FRAME_SIZE,
                         http2::kMaxFramePayloadLengthMin * 2);
    downstreamCodec_.generateSettings(output_);
    parseUpstream();
  }

  settings->setSetting(SettingsId::MAX_FRAME_SIZE,
                       http2::kMaxFramePayloadLengthMin);
  upstreamCodec_.generateHeader(output_, 1, req, true /* eom */);

  parse();
  // session error
  EXPECT_EQ(callbacks_.messageBegin, oversized ? 0 : 1);
  EXPECT_EQ(callbacks_.headersComplete, oversized ? 0 : 1);
  EXPECT_EQ(callbacks_.messageComplete, oversized ? 0 : 1);
  EXPECT_EQ(callbacks_.streamErrors, 0);
  EXPECT_EQ(callbacks_.sessionErrors, oversized ? 1 : 0);
}