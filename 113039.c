TEST_F(HTTP2CodecTest, WebsocketDupProtocol) {
  const std::string kConnect{"CONNECT"};
  const std::string kWebsocketPath{"/websocket"};
  const std::string kSchemeHttps{"https"};
  vector<proxygen::compress::Header> headers = {
    Header::makeHeaderForTest(headers::kMethod, kConnect),
    Header::makeHeaderForTest(headers::kProtocol, headers::kWebsocketString),
    Header::makeHeaderForTest(headers::kProtocol, headers::kWebsocketString),
    Header::makeHeaderForTest(headers::kPath, kWebsocketPath),
    Header::makeHeaderForTest(headers::kScheme, kSchemeHttps),
  };
  HPACKCodec headerCodec(TransportDirection::UPSTREAM);
  auto encodedHeaders = headerCodec.encode(headers);
  http2::writeHeaders(output_,
                      std::move(encodedHeaders),
                      1,
                      folly::none,
                      http2::kNoPadding,
                      false,
                      true);
  parse();
  EXPECT_EQ(callbacks_.messageBegin, 0);
  EXPECT_EQ(callbacks_.headersComplete, 0);
  EXPECT_EQ(callbacks_.messageComplete, 0);
  EXPECT_EQ(callbacks_.streamErrors, 1);
  EXPECT_EQ(callbacks_.sessionErrors, 0);
}