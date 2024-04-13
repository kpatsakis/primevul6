TEST_F(HTTP2CodecTest, DoubleGoawayWithError) {
  SetUpUpstreamTest();
  std::unique_ptr<folly::IOBuf> debugData =
    folly::IOBuf::copyBuffer("debugData");
  downstreamCodec_.generateGoaway(output_, std::numeric_limits<int32_t>::max(),
                                  ErrorCode::ENHANCE_YOUR_CALM,
                                  std::move(debugData));
  EXPECT_FALSE(downstreamCodec_.isWaitingToDrain());
  EXPECT_FALSE(downstreamCodec_.isReusable());
  auto ret = downstreamCodec_.generateGoaway(output_, 0,
                                             ErrorCode::NO_ERROR);
  EXPECT_EQ(ret, 0);

  parseUpstream();
  EXPECT_EQ(callbacks_.goaways, 1);
  EXPECT_EQ(callbacks_.data.move()->moveToFbString(), "debugData");
  EXPECT_EQ(callbacks_.streamErrors, 0);
  EXPECT_EQ(callbacks_.sessionErrors, 0);
}