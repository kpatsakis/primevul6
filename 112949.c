TEST_F(HTTP2CodecTest, JunkAfterConnError) {
  HTTPMessage req = getGetRequest();
  req.getHeaders().add(HTTP_HEADER_USER_AGENT, "coolio");

  // write headers frame for stream 0
  writeFrameHeaderManual(output_, 0, (uint8_t)http2::FrameType::HEADERS, 0, 0);
  // now write a valid headers frame, should never be parsed
  upstreamCodec_.generateHeader(output_, 1, req);

  parse();
  EXPECT_EQ(callbacks_.messageBegin, 0);
  EXPECT_EQ(callbacks_.headersComplete, 0);
  EXPECT_EQ(callbacks_.messageComplete, 0);
  EXPECT_EQ(callbacks_.streamErrors, 0);
  EXPECT_EQ(callbacks_.sessionErrors, 1);
}