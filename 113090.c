TEST_F(HTTP2CodecTest, BasicPriority) {
  auto pri = HTTPMessage::HTTPPriority(0, true, 1);
  upstreamCodec_.generatePriority(output_, 1, pri);

  EXPECT_TRUE(parse());
  EXPECT_EQ(callbacks_.priority, pri);
  EXPECT_EQ(callbacks_.streamErrors, 0);
  EXPECT_EQ(callbacks_.sessionErrors, 0);
}