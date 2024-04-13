TEST_F(HTTP2CodecTest, NoExHeaders) {
  // do not emit ENABLE_EX_HEADERS setting, if disabled
  SetUpUpstreamTest();

  EXPECT_EQ(callbacks_.settings, 0);
  EXPECT_EQ(callbacks_.numSettings, 0);
  EXPECT_EQ(false, downstreamCodec_.supportsExTransactions());

  parseUpstream();

  EXPECT_EQ(callbacks_.settings, 1);
  // only 3 standard settings: HEADER_TABLE_SIZE, ENABLE_PUSH, MAX_FRAME_SIZE.
  EXPECT_EQ(callbacks_.numSettings, 3);
  EXPECT_EQ(false, downstreamCodec_.supportsExTransactions());
}