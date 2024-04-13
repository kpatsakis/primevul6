TEST_F(HTTP2CodecTest, BadHeaders) {
  static const std::string v1("GET");
  static const std::string v2("/");
  static const std::string v3("http");
  static const std::string v4("foo.com");
  static const vector<proxygen::compress::Header> reqHeaders = {
    Header::makeHeaderForTest(headers::kMethod, v1),
    Header::makeHeaderForTest(headers::kPath, v2),
    Header::makeHeaderForTest(headers::kScheme, v3),
    Header::makeHeaderForTest(headers::kAuthority, v4),
  };

  HPACKCodec headerCodec(TransportDirection::UPSTREAM);
  HTTPCodec::StreamID stream = 1;
  // missing fields (missing authority is OK)
  for (size_t i = 0; i < reqHeaders.size(); i++, stream += 2) {
    std::vector<proxygen::compress::Header> allHeaders = reqHeaders;
    allHeaders.erase(allHeaders.begin() + i);
    auto encodedHeaders = headerCodec.encode(allHeaders);
    http2::writeHeaders(output_,
                        std::move(encodedHeaders),
                        stream,
                        folly::none,
                        http2::kNoPadding,
                        true,
                        true);
  }
  // dup fields
  std::string v("foomonkey");
  for (size_t i = 0; i < reqHeaders.size(); i++, stream += 2) {
    std::vector<proxygen::compress::Header> allHeaders = reqHeaders;
    auto h = allHeaders[i];
    h.value = &v;
    allHeaders.push_back(h);
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
  EXPECT_EQ(callbacks_.messageBegin, 1);
  EXPECT_EQ(callbacks_.headersComplete, 1);
  EXPECT_EQ(callbacks_.messageComplete, 1);
  EXPECT_EQ(callbacks_.streamErrors, 7);
  EXPECT_EQ(callbacks_.sessionErrors, 0);
}