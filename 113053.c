TEST_F(HTTP2CodecTest, BasicData) {
  string data("abcde");
  auto buf = folly::IOBuf::copyBuffer(data.data(), data.length());
  upstreamCodec_.generateBody(output_, 2, std::move(buf),
                              HTTPCodec::NoPadding, true);

  parse();
  EXPECT_EQ(callbacks_.messageBegin, 0);
  EXPECT_EQ(callbacks_.headersComplete, 0);
  EXPECT_EQ(callbacks_.messageComplete, 1);
  EXPECT_EQ(callbacks_.bodyCalls, 1);
  EXPECT_EQ(callbacks_.bodyLength, 5);
  EXPECT_EQ(callbacks_.streamErrors, 0);
  EXPECT_EQ(callbacks_.sessionErrors, 0);
  EXPECT_EQ(callbacks_.data.move()->moveToFbString(), data);
}