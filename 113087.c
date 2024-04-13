TEST_F(HTTP2CodecTest, TestMultipleDifferentContentLengthHeaders) {
  // Generate a POST request with two Content-Length headers
  // NOTE: getPostRequest already adds the content-length
  HTTPMessage req = getPostRequest();
  req.getHeaders().add(HTTP_HEADER_CONTENT_LENGTH, "300");
  EXPECT_EQ(req.getHeaders().getNumberOfValues(HTTP_HEADER_CONTENT_LENGTH), 2);

  upstreamCodec_.generateHeader(output_, 1, req, true /* eom */);
  parse();

  // Check that the request fails before the codec finishes parsing the headers
  EXPECT_EQ(callbacks_.streamErrors, 1);
  EXPECT_EQ(callbacks_.headersComplete, 0);
  EXPECT_EQ(callbacks_.lastParseError->getHttpStatusCode(), 400);
}