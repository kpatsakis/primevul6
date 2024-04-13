TEST_F(HTTP2CodecTest, BadHeaderValues) {
  static const std::string v1("--1");
  static const std::string v2("\13\10protocol-attack");
  static const std::string v3("\13");
  static const std::string v4("abc.com\\13\\10");
  static const vector<proxygen::compress::Header> reqHeaders = {
    Header::makeHeaderForTest(headers::kMethod, v1),
    Header::makeHeaderForTest(headers::kPath, v2),
    Header::makeHeaderForTest(headers::kScheme, v3),
    Header::makeHeaderForTest(headers::kAuthority, v4),
  };

  HPACKCodec headerCodec(TransportDirection::UPSTREAM);
  HTTPCodec::StreamID stream = 1;
  for (size_t i = 0; i < reqHeaders.size(); i++, stream += 2) {
    std::vector<proxygen::compress::Header> allHeaders;
    allHeaders.push_back(reqHeaders[i]);
    auto encodedHeaders = headerCodec.encode(allHeaders);
    http2::writeHeaders(output_,
                        std::move(encodedHeaders),
                        stream,
                        folly::none,
                        http2::kNoPadding,
                        true,
                        true);
  }

  parse();
  EXPECT_EQ(callbacks_.messageBegin, 0);
  EXPECT_EQ(callbacks_.headersComplete, 0);
  EXPECT_EQ(callbacks_.messageComplete, 0);
  EXPECT_EQ(callbacks_.streamErrors, 4);
  EXPECT_EQ(callbacks_.sessionErrors, 0);
}