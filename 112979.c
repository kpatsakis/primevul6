TEST_F(HTTP2CodecTest, HTTP2SettingsSuccess) {
  HTTPMessage req = getGetRequest("/guacamole");

  // empty settings
  req.getHeaders().add(http2::kProtocolSettingsHeader, "");
  EXPECT_TRUE(downstreamCodec_.onIngressUpgradeMessage(req));

  // real settings (overwrites empty)
  HTTP2Codec::requestUpgrade(req);
  EXPECT_TRUE(downstreamCodec_.onIngressUpgradeMessage(req));
}