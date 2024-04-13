TEST_F(HTTP2CodecTest, BasicWindow) {
  // This test would fail if the codec had window state
  upstreamCodec_.generateWindowUpdate(output_, 0, 10);
  upstreamCodec_.generateWindowUpdate(output_, 0, http2::kMaxWindowUpdateSize);
  upstreamCodec_.generateWindowUpdate(output_, 1, 12);
  upstreamCodec_.generateWindowUpdate(output_, 1, http2::kMaxWindowUpdateSize);

  parse();
  EXPECT_EQ(callbacks_.windowUpdateCalls, 4);
  EXPECT_EQ(callbacks_.windowUpdates[0],
            std::vector<uint32_t>({10, http2::kMaxWindowUpdateSize}));
  EXPECT_EQ(callbacks_.windowUpdates[1],
            std::vector<uint32_t>({12, http2::kMaxWindowUpdateSize}));
  EXPECT_EQ(callbacks_.streamErrors, 0);
  EXPECT_EQ(callbacks_.sessionErrors, 0);
}