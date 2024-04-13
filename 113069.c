TEST_F(HTTP2CodecTest, TrailersReplyWithNoData) {
  SetUpUpstreamTest();
  HTTPMessage resp;
  resp.setStatusCode(200);
  resp.setStatusMessage("nifty-nice");
  resp.getHeaders().add(HTTP_HEADER_CONTENT_TYPE, "x-coolio");
  downstreamCodec_.generateHeader(output_, 1, resp);

  HTTPHeaders trailers;
  trailers.add("x-trailer-1", "pico-de-gallo");
  downstreamCodec_.generateTrailers(output_, 1, trailers);

  parseUpstream();

  callbacks_.expectMessage(true, 2, 200);
  EXPECT_EQ(callbacks_.bodyCalls, 0);
  EXPECT_EQ(callbacks_.bodyLength, 0);
  const auto& headers = callbacks_.msg->getHeaders();
  EXPECT_TRUE(callbacks_.msg->getHeaders().exists(HTTP_HEADER_DATE));
  EXPECT_EQ("x-coolio", headers.getSingleOrEmpty(HTTP_HEADER_CONTENT_TYPE));
  EXPECT_EQ(1, callbacks_.trailers);
  EXPECT_NE(nullptr, callbacks_.msg->getTrailers());
  EXPECT_EQ("pico-de-gallo",
            callbacks_.msg->getTrailers()->getSingleOrEmpty("x-trailer-1"));
#ifndef NDEBUG
  EXPECT_EQ(upstreamCodec_.getReceivedFrameCount(), 3);
#endif
}