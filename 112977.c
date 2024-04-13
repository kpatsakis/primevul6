TEST_F(HTTP2CodecTest, NoAppByte) {
  const uint8_t noAppByte[] = {0x50, 0x52, 0x49, 0x20, 0x2a, 0x20, 0x48, 0x54,
                               0x54, 0x50, 0x2f, 0x32, 0x2e, 0x30, 0x0d, 0x0a,
                               0x0d, 0x0a, 0x53, 0x4d, 0x0d, 0x0a, 0x0d, 0x0a,
                               0x00, 0x00, 0x56, 0x00, 0x5d, 0x00, 0x00, 0x00,
                               0x01, 0x55, 0x00};
  output_.clear();
  output_.append(noAppByte, sizeof(noAppByte));
  EXPECT_EQ(output_.chainLength(), sizeof(noAppByte));

  EXPECT_TRUE(parse());
  EXPECT_EQ(callbacks_.messageBegin, 0);
  EXPECT_EQ(callbacks_.headersComplete, 0);
  EXPECT_EQ(callbacks_.messageComplete, 0);
  EXPECT_EQ(callbacks_.bodyCalls, 1);
  EXPECT_EQ(callbacks_.bodyLength, 0);
  EXPECT_EQ(callbacks_.streamErrors, 0);
  EXPECT_EQ(callbacks_.sessionErrors, 0);
}