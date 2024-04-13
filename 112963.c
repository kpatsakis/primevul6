TEST_F(HTTP2CodecTest, IgnoreUnknownSettings) {
  auto numSettings = downstreamCodec_.getIngressSettings()->getNumSettings();
  std::deque<SettingPair> settings;
  for (uint32_t i = 200; i < (200 + 1024); i++) {
    settings.push_back(SettingPair(SettingsId(i), i));
  }
  http2::writeSettings(output_, settings);
  parse();

  EXPECT_EQ(callbacks_.settings, 1);
  EXPECT_EQ(callbacks_.sessionErrors, 0);
  EXPECT_EQ(numSettings,
            downstreamCodec_.getIngressSettings()->getNumSettings());
}