TEST_F(HTTP2CodecTest, BadSettings) {
  auto settings = upstreamCodec_.getEgressSettings();
  settings->setSetting(SettingsId::INITIAL_WINDOW_SIZE, 0xffffffff);
  upstreamCodec_.generateSettings(output_);

  parse();
  EXPECT_EQ(callbacks_.settings, 0);
  EXPECT_EQ(callbacks_.streamErrors, 0);
  EXPECT_EQ(callbacks_.sessionErrors, 1);
}