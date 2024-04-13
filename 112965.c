TEST_F(HTTP2CodecTest, BadConnect) {
  std::string v1 = "CONNECT";
  std::string v2 = "somehost:576";
  std::vector<proxygen::compress::Header> goodHeaders = {
    Header::makeHeaderForTest(headers::kMethod, v1),
    Header::makeHeaderForTest(headers::kAuthority, v2),
  };

  // See https://tools.ietf.org/html/rfc7540#section-8.3
  std::string v3 = "/foobar";
  std::vector<proxygen::compress::Header> badHeaders = {
    Header::makeHeaderForTest(headers::kScheme, headers::kHttp),
    Header::makeHeaderForTest(headers::kPath, v3),
  };

  HPACKCodec headerCodec(TransportDirection::UPSTREAM);
  HTTPCodec::StreamID stream = 1;

  for (size_t i = 0; i < badHeaders.size(); i++, stream += 2) {
    auto allHeaders = goodHeaders;
    allHeaders.push_back(badHeaders[i]);
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
  EXPECT_EQ(callbacks_.streamErrors, badHeaders.size());
  EXPECT_EQ(callbacks_.sessionErrors, 0);
}