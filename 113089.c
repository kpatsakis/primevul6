TEST_F(HTTP2CodecTest, BasicSetting) {
  auto settings = upstreamCodec_.getEgressSettings();
  settings->setSetting(SettingsId::MAX_CONCURRENT_STREAMS, 37);
  settings->setSetting(SettingsId::INITIAL_WINDOW_SIZE, 12345);
  upstreamCodec_.generateSettings(output_);

  parse();
  EXPECT_EQ(callbacks_.settings, 1);
  EXPECT_EQ(callbacks_.maxStreams, 37);
  EXPECT_EQ(callbacks_.windowSize, 12345);
  EXPECT_EQ(callbacks_.streamErrors, 0);
  EXPECT_EQ(callbacks_.sessionErrors, 0);
}