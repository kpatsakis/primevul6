TEST_F(HTTP2CodecTest, DataFramePartialDataOnFrameHeaderCall) {
  using namespace testing;
  NiceMock<MockHTTPCodecCallback> mockCallback;
  EXPECT_CALL(mockCallback, onFrameHeader(_, _, _, _, _));

  const size_t bufSize = 10;
  auto buf = makeBuf(bufSize);
  const size_t padding = 10;
  upstreamCodec_.generateBody(output_, 1, buf->clone(), padding, true);
  EXPECT_EQ(output_.chainLength(), 54);

  downstreamCodec_.setCallback(&mockCallback);

  auto ingress = output_.move();
  ingress->coalesce();
  // Copy partial byte to a new buffer
  auto ingress1 = IOBuf::copyBuffer(ingress->data(), 34);
  downstreamCodec_.onIngress(*ingress1);
}