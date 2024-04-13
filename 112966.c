TEST_F(HTTP2CodecTest, DoubleGoaway) {
  parse();
  SetUpUpstreamTest();
  downstreamCodec_.generateGoaway(output_, std::numeric_limits<int32_t>::max(),
                                  ErrorCode::NO_ERROR);
  EXPECT_TRUE(downstreamCodec_.isWaitingToDrain());
  EXPECT_TRUE(downstreamCodec_.isReusable());
  EXPECT_TRUE(downstreamCodec_.isStreamIngressEgressAllowed(0));
  EXPECT_TRUE(downstreamCodec_.isStreamIngressEgressAllowed(1));
  EXPECT_TRUE(downstreamCodec_.isStreamIngressEgressAllowed(2));
  downstreamCodec_.generateGoaway(output_, 0, ErrorCode::NO_ERROR);
  EXPECT_FALSE(downstreamCodec_.isWaitingToDrain());
  EXPECT_FALSE(downstreamCodec_.isReusable());
  EXPECT_TRUE(downstreamCodec_.isStreamIngressEgressAllowed(0));
  EXPECT_FALSE(downstreamCodec_.isStreamIngressEgressAllowed(1));
  EXPECT_TRUE(downstreamCodec_.isStreamIngressEgressAllowed(2));

  EXPECT_TRUE(upstreamCodec_.isStreamIngressEgressAllowed(0));
  EXPECT_TRUE(upstreamCodec_.isStreamIngressEgressAllowed(1));
  EXPECT_TRUE(upstreamCodec_.isStreamIngressEgressAllowed(2));
  parseUpstream();
  EXPECT_TRUE(upstreamCodec_.isStreamIngressEgressAllowed(0));
  EXPECT_FALSE(upstreamCodec_.isStreamIngressEgressAllowed(1));
  EXPECT_TRUE(upstreamCodec_.isStreamIngressEgressAllowed(2));
  EXPECT_EQ(callbacks_.goaways, 2);
  EXPECT_EQ(callbacks_.streamErrors, 0);
  EXPECT_EQ(callbacks_.sessionErrors, 0);

  upstreamCodec_.generateGoaway(output_, 0, ErrorCode::NO_ERROR);
  EXPECT_TRUE(upstreamCodec_.isStreamIngressEgressAllowed(0));
  EXPECT_FALSE(upstreamCodec_.isStreamIngressEgressAllowed(1));
  EXPECT_FALSE(upstreamCodec_.isStreamIngressEgressAllowed(2));
  parse();
  EXPECT_TRUE(downstreamCodec_.isStreamIngressEgressAllowed(0));
  EXPECT_FALSE(downstreamCodec_.isStreamIngressEgressAllowed(1));
  EXPECT_FALSE(downstreamCodec_.isStreamIngressEgressAllowed(2));
}