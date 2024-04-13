TEST_F(HTTP2CodecTest, UnknownFrameType) {
  HTTPMessage req = getGetRequest();
  req.getHeaders().add(HTTP_HEADER_USER_AGENT, "coolio");

  // unknown frame type 17
  writeFrameHeaderManual(output_, 17, 37, 0, 1);
  output_.append("wicked awesome!!!");
  upstreamCodec_.generateHeader(output_, 1, req);

  parse();
  callbacks_.expectMessage(false, 2, ""); // + host
}