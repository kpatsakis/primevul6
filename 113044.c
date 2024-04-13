TEST_F(HTTP2CodecTest, BasicCertificate) {
  uint16_t certId = 17;
  std::unique_ptr<folly::IOBuf> authenticator =
      folly::IOBuf::copyBuffer("authenticatorData");
  upstreamCodec_.generateCertificate(output_, certId, std::move(authenticator));

  parse();
  EXPECT_EQ(callbacks_.certificates, 1);
  EXPECT_EQ(callbacks_.lastCertId, certId);
  EXPECT_EQ(callbacks_.data.move()->moveToFbString(), "authenticatorData");
  EXPECT_EQ(callbacks_.streamErrors, 0);
  EXPECT_EQ(callbacks_.sessionErrors, 0);
}