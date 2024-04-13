TEST_F(HTTP2CodecTest, TrailersReplyContinuation) {
  SetUpUpstreamTest();
  HTTPMessage resp;
  resp.setStatusCode(200);
  downstreamCodec_.generateHeader(output_, 1, resp);

  HTTPHeaders trailers;
  trailers.add("x-trailer-1", "pico-de-gallo");
  trailers.add("x-huge-trailer",
               std::string(http2::kMaxFramePayloadLengthMin, '!'));
  downstreamCodec_.generateTrailers(output_, 1, trailers);

  parseUpstream();

  EXPECT_EQ(callbacks_.messageBegin, 1);
  EXPECT_EQ(callbacks_.headersComplete, 1);
  EXPECT_EQ(callbacks_.messageComplete, 1);
  EXPECT_EQ(callbacks_.streamErrors, 0);
  EXPECT_EQ(callbacks_.sessionErrors, 0);
  EXPECT_NE(callbacks_.msg, nullptr);
  EXPECT_EQ(callbacks_.msg->getStatusCode(), 200);
  EXPECT_EQ(1, callbacks_.trailers);
  EXPECT_NE(nullptr, callbacks_.msg->getTrailers());
  EXPECT_EQ("pico-de-gallo",
            callbacks_.msg->getTrailers()->getSingleOrEmpty("x-trailer-1"));
  EXPECT_EQ(std::string(http2::kMaxFramePayloadLengthMin, '!'),
            callbacks_.msg->getTrailers()->getSingleOrEmpty("x-huge-trailer"));
#ifndef NDEBUG
  EXPECT_EQ(upstreamCodec_.getReceivedFrameCount(), 4);
#endif
}