TEST_F(HTTP2CodecTest, TrailersReplyMissingContinuation) {
  SetUpUpstreamTest();
  HTTPMessage resp;
  resp.setStatusCode(200);
  downstreamCodec_.generateHeader(output_, 1, resp);

  HTTPHeaders trailers;
  trailers.add("x-trailer-1", "pico-de-gallo");
  trailers.add("x-huge-trailer",
               std::string(http2::kMaxFramePayloadLengthMin, '!'));
  downstreamCodec_.generateTrailers(output_, 1, trailers);
  // empirically determined the size of continuation frame, and strip it
  output_.trimEnd(http2::kFrameHeaderSize + 4132);

  // insert a non-continuation (but otherwise valid) frame
  http2::writeGoaway(output_, 17, ErrorCode::ENHANCE_YOUR_CALM);

  parseUpstream();

  EXPECT_EQ(callbacks_.messageBegin, 1);
  EXPECT_EQ(callbacks_.headersComplete, 1);
  EXPECT_EQ(callbacks_.messageComplete, 0);
  EXPECT_EQ(callbacks_.streamErrors, 0);
  EXPECT_EQ(callbacks_.sessionErrors, 1);
#ifndef NDEBUG
  EXPECT_EQ(upstreamCodec_.getReceivedFrameCount(), 4);
#endif
}