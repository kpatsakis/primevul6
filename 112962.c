TEST_F(HTTP2CodecTest, TrailersNoBody) {
  HTTPMessage req = getGetRequest("/guacamole");
  req.getHeaders().add(HTTP_HEADER_USER_AGENT, "coolio");
  upstreamCodec_.generateHeader(output_, 1, req);

  HTTPHeaders trailers;
  trailers.add("x-trailer-1", "pico-de-gallo");
  upstreamCodec_.generateTrailers(output_, 1, trailers);

  parse();

  EXPECT_EQ(callbacks_.messageBegin, 1);
  EXPECT_EQ(callbacks_.headersComplete, 1);
  EXPECT_EQ(callbacks_.bodyCalls, 0);
  EXPECT_EQ(callbacks_.bodyLength, 0);
  EXPECT_EQ(callbacks_.trailers, 1);
  EXPECT_NE(nullptr, callbacks_.msg->getTrailers());
  EXPECT_EQ("pico-de-gallo",
            callbacks_.msg->getTrailers()->getSingleOrEmpty("x-trailer-1"));
  EXPECT_EQ(callbacks_.messageComplete, 1);
  EXPECT_EQ(callbacks_.streamErrors, 0);
  EXPECT_EQ(callbacks_.sessionErrors, 0);
#ifndef NDEBUG
  EXPECT_EQ(downstreamCodec_.getReceivedFrameCount(), 2);
#endif
}