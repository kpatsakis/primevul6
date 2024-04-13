TEST_F(HTTP2CodecTest, BadPushSettings) {
  auto settings = downstreamCodec_.getEgressSettings();
  settings->clearSettings();
  settings->setSetting(SettingsId::ENABLE_PUSH, 0);
  SetUpUpstreamTest();

  parseUpstream([&] (IOBuf* ingress) {
      EXPECT_EQ(ingress->computeChainDataLength(), http2::kFrameHeaderSize);
    });
  EXPECT_FALSE(upstreamCodec_.supportsPushTransactions());
  // Only way to disable push for downstreamCodec_ is to read
  // ENABLE_PUSH:0 from client
  EXPECT_TRUE(downstreamCodec_.supportsPushTransactions());
  EXPECT_EQ(callbacks_.settings, 1);
  EXPECT_EQ(callbacks_.streamErrors, 0);
  EXPECT_EQ(callbacks_.sessionErrors, 0);
}