TEST_F(HTTP2CodecTest, BasicCertificateRequest) {
  uint16_t requestId = 17;
  std::unique_ptr<folly::IOBuf> authRequest =
      folly::IOBuf::copyBuffer("authRequestData");
  upstreamCodec_.generateCertificateRequest(
      output_, requestId, std::move(authRequest));

  parse();
  EXPECT_EQ(callbacks_.certificateRequests, 1);
  EXPECT_EQ(callbacks_.lastCertRequestId, requestId);
  EXPECT_EQ(callbacks_.data.move()->moveToFbString(), "authRequestData");
  EXPECT_EQ(callbacks_.streamErrors, 0);
  EXPECT_EQ(callbacks_.sessionErrors, 0);
}