TEST_F(HTTP2CodecTest, BadPseudoHeaders) {
  static const std::string v1("POST");
  static const std::string v2("http");
  static const std::string n3("foo");
  static const std::string v3("bar");
  static const std::string v4("/");
  static const vector<proxygen::compress::Header> reqHeaders = {
    Header::makeHeaderForTest(headers::kMethod, v1),
    Header::makeHeaderForTest(headers::kScheme, v2),
    Header::makeHeaderForTest(n3, v3),
    Header::makeHeaderForTest(headers::kPath, v4),
  };

  HPACKCodec headerCodec(TransportDirection::UPSTREAM);
  HTTPCodec::StreamID stream = 1;
  std::vector<proxygen::compress::Header> allHeaders = reqHeaders;
  auto encodedHeaders = headerCodec.encode(allHeaders);
  http2::writeHeaders(output_,
                      std::move(encodedHeaders),
                      stream,
                      folly::none,
                      http2::kNoPadding,
                      true,
                      true);

  parse();
  EXPECT_EQ(callbacks_.messageBegin, 0);
  EXPECT_EQ(callbacks_.headersComplete, 0);
  EXPECT_EQ(callbacks_.messageComplete, 0);
  EXPECT_EQ(callbacks_.streamErrors, 1);
  EXPECT_EQ(callbacks_.sessionErrors, 0);
}