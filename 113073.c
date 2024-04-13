TEST_F(HTTP2CodecTest, TrailersWithPseudoHeaders) {
  HTTPMessage req = getGetRequest("/guacamole");
  req.getHeaders().add(HTTP_HEADER_USER_AGENT, "coolio");
  upstreamCodec_.generateHeader(output_, 1, req);

  string data("abcde");
  auto buf = folly::IOBuf::copyBuffer(data.data(), data.length());
  upstreamCodec_.generateBody(
      output_, 1, std::move(buf), HTTPCodec::NoPadding, false /* eom */);

  HPACKCodec headerCodec(TransportDirection::UPSTREAM);
  std::string post("POST");
  std::vector<proxygen::compress::Header> trailers = {
      Header::makeHeaderForTest(headers::kMethod, post)};
  auto encodedTrailers = headerCodec.encode(trailers);
  http2::writeHeaders(output_,
                      std::move(encodedTrailers),
                      1,
                      folly::none,
                      http2::kNoPadding,
                      true,
                      true);

  parse();

  EXPECT_EQ(callbacks_.messageBegin, 1);
  EXPECT_EQ(callbacks_.headersComplete, 1);
  EXPECT_EQ(callbacks_.bodyCalls, 1);
  EXPECT_EQ(callbacks_.bodyLength, 5);
  EXPECT_EQ(callbacks_.trailers, 0);
  EXPECT_EQ(nullptr, callbacks_.msg->getTrailers());
  EXPECT_EQ(callbacks_.messageComplete, 0);
  EXPECT_EQ(callbacks_.streamErrors, 1);
}