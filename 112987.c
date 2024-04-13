TEST_F(HTTP2CodecTest, GoawayReply) {
  upstreamCodec_.generateGoaway(output_, 0, ErrorCode::NO_ERROR);

  parse();
  EXPECT_EQ(callbacks_.goaways, 1);
  EXPECT_EQ(callbacks_.streamErrors, 0);
  EXPECT_EQ(callbacks_.sessionErrors, 0);

  SetUpUpstreamTest();
  HTTPMessage resp;
  resp.setStatusCode(200);
  resp.setStatusMessage("nifty-nice");
  downstreamCodec_.generateHeader(output_, 1, resp);
  downstreamCodec_.generateEOM(output_, 1);
  parseUpstream();
  callbacks_.expectMessage(true, 1, 200);
  EXPECT_TRUE(callbacks_.msg->getHeaders().exists(HTTP_HEADER_DATE));
}