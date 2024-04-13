TEST_F(HTTP2CodecTest, SettingsAck) {
  upstreamCodec_.generateSettingsAck(output_);

  parse();
  EXPECT_EQ(callbacks_.settings, 0);
  EXPECT_EQ(callbacks_.settingsAcks, 1);
  EXPECT_EQ(callbacks_.streamErrors, 0);
  EXPECT_EQ(callbacks_.sessionErrors, 0);
}