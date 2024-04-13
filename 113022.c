TEST_F(HTTP2CodecTest, WebsocketIncorrectResponse) {
  parse();
  SetUpUpstreamTest();
  parseUpstream();

  output_.clear();
  HTTPMessage req = getGetRequest("/apples");
  req.setSecure(true);
  req.setEgressWebsocketUpgrade();
  upstreamCodec_.generateHeader(output_, 1, req, false);
  parse();

  output_.clear();
  HTTPMessage resp;
  resp.setStatusCode(201);
  resp.setStatusMessage("OK");
  downstreamCodec_.generateHeader(output_, 1, resp);
  parseUpstream();
  EXPECT_EQ(callbacks_.streamErrors, 1);
}