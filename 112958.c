TEST_F(HTTP2CodecTest, TrailersReplyWithPseudoHeaders) {
  SetUpUpstreamTest();
  HTTPMessage resp;
  resp.setStatusCode(200);
  resp.setStatusMessage("nifty-nice");
  resp.getHeaders().add(HTTP_HEADER_CONTENT_TYPE, "x-coolio");
  downstreamCodec_.generateHeader(output_, 1, resp);

  string data("abcde");
  auto buf = folly::IOBuf::copyBuffer(data.data(), data.length());
  downstreamCodec_.generateBody(
      output_, 1, std::move(buf), HTTPCodec::NoPadding, false);

  HPACKCodec headerCodec(TransportDirection::DOWNSTREAM);
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
  parseUpstream();

  EXPECT_EQ(callbacks_.messageBegin, 1);
  EXPECT_EQ(callbacks_.headersComplete, 1);
  EXPECT_EQ(callbacks_.trailers, 0);
  EXPECT_EQ(nullptr, callbacks_.msg->getTrailers());
  EXPECT_EQ(callbacks_.streamErrors, 1);
  EXPECT_EQ(callbacks_.sessionErrors, 0);
}