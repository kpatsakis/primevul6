TEST_F(HTTP2CodecTest, WebsocketUpgrade) {
  HTTPMessage req = getGetRequest("/apples");
  req.setSecure(true);
  req.setEgressWebsocketUpgrade();

  upstreamCodec_.generateHeader(output_, 1, req, false);
  parse();

  EXPECT_TRUE(callbacks_.msg->isIngressWebsocketUpgrade());
  EXPECT_NE(nullptr, callbacks_.msg->getUpgradeProtocol());
  EXPECT_EQ(headers::kWebsocketString, *callbacks_.msg->getUpgradeProtocol());
}