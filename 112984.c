TEST_F(HTTP2CodecTest, TrailersContinuation) {
  HTTPMessage req = getGetRequest("/guacamole");
  upstreamCodec_.generateHeader(output_, 1, req);

  HTTPHeaders trailers;
  trailers.add("x-trailer-1", "pico-de-gallo");
  trailers.add("x-huge-trailer",
               std::string(http2::kMaxFramePayloadLengthMin, '!'));
  upstreamCodec_.generateTrailers(output_, 1, trailers);

  parse();

  EXPECT_EQ(callbacks_.messageBegin, 1);
  EXPECT_EQ(callbacks_.headersComplete, 1);
  EXPECT_EQ(callbacks_.messageComplete, 1);
  EXPECT_EQ(callbacks_.streamErrors, 0);
  EXPECT_EQ(callbacks_.sessionErrors, 0);
  EXPECT_NE(callbacks_.msg, nullptr);
  EXPECT_EQ(callbacks_.trailers, 1);
  EXPECT_NE(callbacks_.msg->getTrailers(), nullptr);
  EXPECT_EQ("pico-de-gallo",
            callbacks_.msg->getTrailers()->getSingleOrEmpty("x-trailer-1"));
  EXPECT_EQ(std::string(http2::kMaxFramePayloadLengthMin, '!'),
            callbacks_.msg->getTrailers()->getSingleOrEmpty("x-huge-trailer"));
#ifndef NDEBUG
  EXPECT_EQ(downstreamCodec_.getReceivedFrameCount(), 3);
#endif
}