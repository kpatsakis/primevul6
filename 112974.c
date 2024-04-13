TEST_F(HTTP2CodecTest, IgnoreExHeadersIfNotEnabled) {
  downstreamCodec_.getEgressSettings()->setSetting(
      SettingsId::ENABLE_EX_HEADERS, 0);

  HTTPMessage req = getGetRequest("/guacamole");
  downstreamCodec_.generateExHeader(output_, 3, req,
                                    HTTPCodec::ExAttributes(1, true));

  parse();
  EXPECT_EQ(callbacks_.streamErrors, 0);
  EXPECT_EQ(callbacks_.sessionErrors, 0);
}