TEST_F(HTTP2CodecTest, BasicConnect) {
  std::string authority = "myhost:1234";
  HTTPMessage request;
  request.setMethod(HTTPMethod::CONNECT);
  request.getHeaders().add(proxygen::HTTP_HEADER_HOST, authority);
  upstreamCodec_.generateHeader(output_, 1, request, false /* eom */);

  parse();
  callbacks_.expectMessage(false, 1, "");
  EXPECT_EQ(HTTPMethod::CONNECT, callbacks_.msg->getMethod());
  const auto& headers = callbacks_.msg->getHeaders();
  EXPECT_EQ(authority, headers.getSingleOrEmpty(proxygen::HTTP_HEADER_HOST));
}