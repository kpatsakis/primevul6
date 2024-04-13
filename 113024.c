TEST_F(HTTP2CodecTest, BadGoaway) {
  std::unique_ptr<folly::IOBuf> debugData =
    folly::IOBuf::copyBuffer("debugData");
  upstreamCodec_.generateGoaway(output_, 17, ErrorCode::ENHANCE_YOUR_CALM,
                                std::move(debugData));
  EXPECT_DEATH_NO_CORE(upstreamCodec_.generateGoaway(
                         output_, 27, ErrorCode::ENHANCE_YOUR_CALM), ".*");
}