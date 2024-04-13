TEST_F(HTTP2CodecTest, InvalidExHeadersSetting) {
  // enable EX_HEADERS on egress
  downstreamCodec_.getEgressSettings()->setSetting(
      SettingsId::ENABLE_EX_HEADERS, 1);

  // attempt to set a invalid ENABLE_EX_HEADERS value
  http2::writeSettings(output_,
                      {SettingPair(SettingsId::ENABLE_EX_HEADERS, 110)});
  parse();

  EXPECT_EQ(callbacks_.sessionErrors, 1);
}