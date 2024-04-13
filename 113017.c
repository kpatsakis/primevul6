TEST_F(HTTP2CodecTest, CleartextUpgrade) {
  HTTPMessage req = getGetRequest("/guacamole");
  req.getHeaders().add(HTTP_HEADER_USER_AGENT, "coolio");
  HTTP2Codec::requestUpgrade(req);
  EXPECT_EQ(req.getHeaders().getSingleOrEmpty(HTTP_HEADER_UPGRADE), "h2c");
  EXPECT_TRUE(req.checkForHeaderToken(HTTP_HEADER_CONNECTION,
                                      "Upgrade", false));
  EXPECT_TRUE(req.checkForHeaderToken(
                HTTP_HEADER_CONNECTION,
                http2::kProtocolSettingsHeader.c_str(), false));
  EXPECT_GT(
    req.getHeaders().getSingleOrEmpty(http2::kProtocolSettingsHeader).length(),
    0);
}