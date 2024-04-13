TEST_F(HTTP2CodecTest, WebsocketBadHeader) {
  const std::string kConnect{"CONNECT"};
  const std::string kWebsocketPath{"/websocket"};
  const std::string kSchemeHttps{"https"};
  vector<proxygen::compress::Header> reqHeaders = {
    Header::makeHeaderForTest(headers::kMethod, kConnect),
    Header::makeHeaderForTest(headers::kProtocol, headers::kWebsocketString),
  };
  vector<proxygen::compress::Header> optionalHeaders = {
    Header::makeHeaderForTest(headers::kPath, kWebsocketPath),
    Header::makeHeaderForTest(headers::kScheme, kSchemeHttps),
  };

  HPACKCodec headerCodec(TransportDirection::UPSTREAM);
  int stream = 1;
  for (size_t i = 0; i < optionalHeaders.size(); ++i, stream += 2) {
    auto headers = reqHeaders;
    headers.push_back(optionalHeaders[i]);
    auto encodedHeaders = headerCodec.encode(headers);
    http2::writeHeaders(output_,
                        std::move(encodedHeaders),
                        stream,
                        folly::none,
                        http2::kNoPadding,
                        false,
                        true);
    parse();
  }

  EXPECT_EQ(callbacks_.messageBegin, 0);
  EXPECT_EQ(callbacks_.headersComplete, 0);
  EXPECT_EQ(callbacks_.messageComplete, 0);
  EXPECT_EQ(callbacks_.streamErrors, optionalHeaders.size());
  EXPECT_EQ(callbacks_.sessionErrors, 0);
}