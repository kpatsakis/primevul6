TEST_F(HTTP2CodecTest, BadSettingsTableSize) {
  auto settings = upstreamCodec_.getEgressSettings();
  settings->setSetting(SettingsId::HEADER_TABLE_SIZE, 8192);
  // This sets the max decoder table size to 8k
  upstreamCodec_.generateSettings(output_);

  parse();
  EXPECT_EQ(callbacks_.settings, 1);
  EXPECT_EQ(callbacks_.streamErrors, 0);
  EXPECT_EQ(callbacks_.sessionErrors, 0);

  callbacks_.reset();

  // Attempt to set a new max table size.  This is a no-op because the first,
  // setting is unacknowledged.  The upstream encoder will up the table size to
  // 8k per the first settings frame and the HPACK codec will send a code to
  // update the decoder.
  settings->setSetting(SettingsId::HEADER_TABLE_SIZE, 4096);

  HTTPMessage resp;
  resp.setStatusCode(200);
  resp.setStatusMessage("nifty-nice");
  resp.getHeaders().add(HTTP_HEADER_CONTENT_TYPE, "x-coolio");
  SetUpUpstreamTest();
  downstreamCodec_.generateHeader(output_, 1, resp);

  parseUpstream();
  EXPECT_EQ(callbacks_.streamErrors, 0);
  EXPECT_EQ(callbacks_.sessionErrors, 1);
  EXPECT_EQ(callbacks_.messageBegin, 0);
  EXPECT_EQ(callbacks_.headersComplete, 0);
}