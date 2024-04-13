TEST_F(HTTP2CodecTest, EnableExHeadersSetting) {
  // enable EX_HEADERS on egress
  downstreamCodec_.getEgressSettings()->setSetting(
      SettingsId::ENABLE_EX_HEADERS, 1);

  auto ptr = downstreamCodec_.getEgressSettings()->getSetting(
      SettingsId::ENABLE_EX_HEADERS);
  EXPECT_EQ(1, ptr->value);

  ptr = downstreamCodec_.getIngressSettings()->getSetting(
      SettingsId::ENABLE_EX_HEADERS);
  EXPECT_EQ(nullptr, ptr);
  EXPECT_EQ(false, downstreamCodec_.supportsExTransactions());

  // attempt to enable EX_HEADERS on ingress
  http2::writeSettings(output_,
                      {SettingPair(SettingsId::ENABLE_EX_HEADERS, 1)});
  parse();

  EXPECT_EQ(callbacks_.settings, 1);
  EXPECT_EQ(callbacks_.sessionErrors, 0);
  ptr = downstreamCodec_.getIngressSettings()->getSetting(
      SettingsId::ENABLE_EX_HEADERS);
  EXPECT_EQ(1, ptr->value);
  EXPECT_EQ(true, downstreamCodec_.supportsExTransactions());

  // attempt to disable EX_HEADERS on ingress
  callbacks_.reset();
  http2::writeSettings(output_,
                      {SettingPair(SettingsId::ENABLE_EX_HEADERS, 0)});
  parse();

  EXPECT_EQ(callbacks_.settings, 1);
  EXPECT_EQ(callbacks_.sessionErrors, 0);
  ptr = downstreamCodec_.getIngressSettings()->getSetting(
      SettingsId::ENABLE_EX_HEADERS);
  EXPECT_EQ(0, ptr->value);
  EXPECT_EQ(false, downstreamCodec_.supportsExTransactions());
}