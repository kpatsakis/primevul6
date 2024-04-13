TEST_F(HTTP2CodecTest, BadHeadersReply) {
  static const std::string v1("200");
  static const vector<proxygen::compress::Header> respHeaders = {
    Header::makeHeaderForTest(headers::kStatus, v1),
  };

  HPACKCodec headerCodec(TransportDirection::DOWNSTREAM);
  HTTPCodec::StreamID stream = 1;
  // missing fields (missing authority is OK)
  for (size_t i = 0; i < respHeaders.size(); i++, stream += 2) {
    std::vector<proxygen::compress::Header> allHeaders = respHeaders;
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
  for (size_t i = 0; i < respHeaders.size(); i++, stream += 2) {
    std::vector<proxygen::compress::Header> allHeaders = respHeaders;
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
  EXPECT_EQ(callbacks_.messageBegin, 0);
  EXPECT_EQ(callbacks_.headersComplete, 0);
  EXPECT_EQ(callbacks_.messageComplete, 0);
  EXPECT_EQ(callbacks_.streamErrors, 2);
  EXPECT_EQ(callbacks_.sessionErrors, 0);
}