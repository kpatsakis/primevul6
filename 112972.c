TEST_F(HTTP2CodecTest, DataFramePartialDataWithNoAppByte) {
  const size_t bufSize = 10;
  auto buf = makeBuf(bufSize);
  const size_t padding = 10;
  upstreamCodec_.generateBody(output_, 1, buf->clone(), padding, true);
  EXPECT_EQ(output_.chainLength(), 54);

  auto ingress = output_.move();
  ingress->coalesce();
  // Copy up to the padding length byte to a new buffer
  auto ingress1 = IOBuf::copyBuffer(ingress->data(), 34);
  size_t parsed = downstreamCodec_.onIngress(*ingress1);
  // The 34th byte is the padding length byte which should not be parsed
  EXPECT_EQ(parsed, 33);
  // Copy from the padding length byte to the end
  auto ingress2 = IOBuf::copyBuffer(ingress->data() + 33, 21);
  parsed = downstreamCodec_.onIngress(*ingress2);
  // The padding length byte should be parsed this time along with 10 bytes of
  // application data and 10 bytes of padding
  EXPECT_EQ(parsed, 21);

  EXPECT_EQ(callbacks_.messageBegin, 0);
  EXPECT_EQ(callbacks_.headersComplete, 0);
  EXPECT_EQ(callbacks_.messageComplete, 1);
  EXPECT_EQ(callbacks_.bodyCalls, 1);
  EXPECT_EQ(callbacks_.bodyLength, bufSize);
  // Total padding is the padding length byte and the padding bytes
  EXPECT_EQ(callbacks_.paddingBytes, padding + 1);
  EXPECT_EQ(callbacks_.streamErrors, 0);
  EXPECT_EQ(callbacks_.sessionErrors, 0);
  EXPECT_EQ(callbacks_.data.move()->moveToFbString(), buf->moveToFbString());
}