TEST_F(HTTP2CodecTest, Normal1024Continuation) {
  HTTPMessage req = getGetRequest();
  string bigval(8691, '!');
  bigval.append(8691, ' ');
  req.getHeaders().add("x-headr", bigval);
  req.setHTTP2Priority(HTTPMessage::HTTPPriority(0, false, 7));
  upstreamCodec_.generateHeader(output_, 1, req);

  parse();
  callbacks_.expectMessage(false, -1, "/");
  const auto& headers = callbacks_.msg->getHeaders();
  EXPECT_EQ(bigval, headers.getSingleOrEmpty("x-headr"));
  EXPECT_EQ(callbacks_.messageBegin, 1);
  EXPECT_EQ(callbacks_.headersComplete, 1);
  EXPECT_EQ(callbacks_.messageComplete, 0);
  EXPECT_EQ(callbacks_.streamErrors, 0);
  EXPECT_EQ(callbacks_.sessionErrors, 0);

  upstreamCodec_.generateSettingsAck(output_);
  parse();
  EXPECT_EQ(callbacks_.settingsAcks, 1);
}