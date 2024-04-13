TEST_F(HTTP2CodecTest, EmptyHeaderName) {
  output_.append(IOBuf::copyBuffer(kBufEmptyHeader, sizeof(kBufEmptyHeader)));
  parse();
  EXPECT_EQ(callbacks_.messageBegin, 0);
  EXPECT_EQ(callbacks_.headersComplete, 0);
  EXPECT_EQ(callbacks_.messageComplete, 0);
  EXPECT_EQ(callbacks_.streamErrors, 1);
  EXPECT_EQ(callbacks_.sessionErrors, 0);
}