TEST_F(HTTP2CodecTest, HTTP2EnableConnect) {
  SetUpUpstreamTest();
  // egress settings have no connect settings.
  auto ws_enable = upstreamCodec_.getEgressSettings()->getSetting(
      SettingsId::ENABLE_CONNECT_PROTOCOL);
  // enable connect settings, and check.
  upstreamCodec_.getEgressSettings()->setSetting(
      SettingsId::ENABLE_CONNECT_PROTOCOL, 1);
  ws_enable = upstreamCodec_.getEgressSettings()->getSetting(
      SettingsId::ENABLE_CONNECT_PROTOCOL);
  EXPECT_EQ(ws_enable->value, 1);
  // generateSettings.
  // pass the buffer to be parsed by the codec and check for ingress settings.
  upstreamCodec_.generateSettings(output_);
  parseUpstream();
  EXPECT_EQ(1, upstreamCodec_.peerHasWebsockets());
}