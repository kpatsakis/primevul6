TEST_F(HTTP2CodecTest, LongData) {
  // Hack the max frame size artificially low
  HTTPSettings* settings = (HTTPSettings*)upstreamCodec_.getIngressSettings();
  settings->setSetting(SettingsId::MAX_FRAME_SIZE, 16);
  auto buf = makeBuf(100);
  upstreamCodec_.generateBody(output_, 1, buf->clone(), HTTPCodec::NoPadding,
                              true);

  parse();
  EXPECT_EQ(callbacks_.messageBegin, 0);
  EXPECT_EQ(callbacks_.headersComplete, 0);
  EXPECT_EQ(callbacks_.messageComplete, 1);
  EXPECT_EQ(callbacks_.bodyCalls, 7);
  EXPECT_EQ(callbacks_.bodyLength, 100);
  EXPECT_EQ(callbacks_.streamErrors, 0);
  EXPECT_EQ(callbacks_.sessionErrors, 0);
  EXPECT_EQ(callbacks_.data.move()->moveToFbString(), buf->moveToFbString());
}