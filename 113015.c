TEST_F(HTTP2CodecTest, NormalSizeFrame) {
  testFrameSizeLimit(false);
}