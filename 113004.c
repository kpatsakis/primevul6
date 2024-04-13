TEST_F(HTTP2CodecTest, BasicGoaway) {
  std::unique_ptr<folly::IOBuf> debugData =
    folly::IOBuf::copyBuffer("debugData");
  upstreamCodec_.generateGoaway(output_, 17, ErrorCode::ENHANCE_YOUR_CALM,
                                std::move(debugData));

  parse();
  EXPECT_EQ(callbacks_.goaways, 1);
  EXPECT_EQ(callbacks_.data.move()->moveToFbString(), "debugData");
  EXPECT_EQ(callbacks_.streamErrors, 0);
  EXPECT_EQ(callbacks_.sessionErrors, 0);
}