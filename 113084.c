TEST_F(HTTP2CodecTest, BadContinuation) {
  // CONTINUATION with no preceding HEADERS
  auto fakeHeaders = makeBuf(5);
  http2::writeContinuation(output_, 3, true, std::move(fakeHeaders));

  parse();
  EXPECT_EQ(callbacks_.messageBegin, 0);
  EXPECT_EQ(callbacks_.headersComplete, 0);
  EXPECT_EQ(callbacks_.messageComplete, 0);
  EXPECT_EQ(callbacks_.streamErrors, 0);
  EXPECT_EQ(callbacks_.sessionErrors, 1);
}