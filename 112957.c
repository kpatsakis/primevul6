TEST_F(HTTP2CodecTest, ExHeadersWithPriority) {
  downstreamCodec_.getEgressSettings()->setSetting(
      SettingsId::ENABLE_EX_HEADERS, 1);
  proxygen::http2::writeSettings(
      output_, {{proxygen::SettingsId::ENABLE_EX_HEADERS, 1}});

  auto req = getGetRequest();
  auto pri = HTTPMessage::HTTPPriority(0, false, 7);
  req.setHTTP2Priority(pri);
  upstreamCodec_.generateExHeader(output_, 3, req,
                                  HTTPCodec::ExAttributes(1, true));

  parse();
  EXPECT_EQ(callbacks_.msg->getHTTP2Priority(), pri);
  EXPECT_EQ(callbacks_.streamErrors, 0);
  EXPECT_EQ(callbacks_.sessionErrors, 0);
}