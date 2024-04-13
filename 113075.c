TEST_F(HTTP2CodecTest, HTTP2SettingsFailure) {
  HTTPMessage req = getGetRequest("/guacamole");
  // no settings
  EXPECT_FALSE(downstreamCodec_.onIngressUpgradeMessage(req));

  HTTPHeaders& headers = req.getHeaders();

  // Not base64_url settings
  headers.set(http2::kProtocolSettingsHeader, "????");
  EXPECT_FALSE(downstreamCodec_.onIngressUpgradeMessage(req));
  headers.set(http2::kProtocolSettingsHeader, "AAA");
  EXPECT_FALSE(downstreamCodec_.onIngressUpgradeMessage(req));

  // Too big
  string bigSettings((http2::kMaxFramePayloadLength + 1) * 4 / 3, 'A');
  headers.set(http2::kProtocolSettingsHeader, bigSettings);
  EXPECT_FALSE(downstreamCodec_.onIngressUpgradeMessage(req));

  // Malformed (not a multiple of 6)
  headers.set(http2::kProtocolSettingsHeader, "AAAA");
  EXPECT_FALSE(downstreamCodec_.onIngressUpgradeMessage(req));

  // Two headers
  headers.set(http2::kProtocolSettingsHeader, "AAAAAAAA");
  headers.add(http2::kProtocolSettingsHeader, "AAAAAAAA");
  EXPECT_FALSE(downstreamCodec_.onIngressUpgradeMessage(req));
}