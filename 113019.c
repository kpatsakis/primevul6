TEST_F(HTTP2CodecTest, TestMultipleIdenticalContentLengthHeaders) {
  // Generate a POST request with two Content-Length headers
  // NOTE: getPostRequest already adds the content-length
  HTTPMessage req = getPostRequest();
  req.getHeaders().add("content-length", "200");
  EXPECT_EQ(req.getHeaders().getNumberOfValues("content-length"), 2);

  upstreamCodec_.generateHeader(output_, 1, req, true /* eom */);
  parse();

  // Check that the headers parsing completes correctly
  EXPECT_EQ(callbacks_.streamErrors, 0);
  EXPECT_EQ(callbacks_.headersComplete, 1);
}