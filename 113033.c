TEST_F(HTTP2CodecTest, GoawayHandling) {
  auto settings = upstreamCodec_.getEgressSettings();
  settings->setSetting(SettingsId::ENABLE_PUSH, 1);
  upstreamCodec_.generateSettings(output_);

  // send request
  HTTPMessage req = getGetRequest();
  HTTPHeaderSize size;
  size.uncompressed = size.compressed = 0;
  upstreamCodec_.generateHeader(output_, 1, req, true, &size);
  EXPECT_GT(size.uncompressed, 0);
  parse();
  callbacks_.expectMessage(true, 1, "/");
  callbacks_.reset();

  SetUpUpstreamTest();
  // drain after this message
  downstreamCodec_.generateGoaway(output_, 1, ErrorCode::NO_ERROR);
  parseUpstream();
  // upstream cannot generate id > 1
  upstreamCodec_.generateHeader(output_, 3, req, false, &size);
  EXPECT_EQ(size.uncompressed, 0);
  upstreamCodec_.generateWindowUpdate(output_, 3, 100);
  upstreamCodec_.generateBody(output_, 3, makeBuf(10), HTTPCodec::NoPadding,
                              false);
  upstreamCodec_.generatePriority(output_, 3,
                                  HTTPMessage::HTTPPriority(0, true, 1));
  upstreamCodec_.generateEOM(output_, 3);
  upstreamCodec_.generateRstStream(output_, 3, ErrorCode::CANCEL);
  EXPECT_EQ(output_.chainLength(), 0);

  // send a push promise that will be rejected by downstream
  req.getHeaders().add("foomonkey", "george");
  downstreamCodec_.generatePushPromise(output_, 2, req, 1, false, &size);
  EXPECT_GT(size.uncompressed, 0);
  HTTPMessage resp;
  resp.setStatusCode(200);
  // send a push response that will be ignored
  downstreamCodec_.generateHeader(output_, 2, resp, false, &size);
  // window update for push doesn't make any sense, but whatever
  downstreamCodec_.generateWindowUpdate(output_, 2, 100);
  downstreamCodec_.generateBody(output_, 2, makeBuf(10), HTTPCodec::NoPadding,
                                false);
  writeFrameHeaderManual(output_, 20, (uint8_t)http2::FrameType::DATA, 0, 2);
  output_.append(makeBuf(10));

  // tell the upstream no pushing, and parse the first batch
  IOBufQueue dummy;
  upstreamCodec_.generateGoaway(dummy, 0, ErrorCode::NO_ERROR);
  parseUpstream();

  output_.append(makeBuf(10));
  downstreamCodec_.generatePriority(output_, 2,
                                    HTTPMessage::HTTPPriority(0, true, 1));
  downstreamCodec_.generateEOM(output_, 2);
  downstreamCodec_.generateRstStream(output_, 2, ErrorCode::CANCEL);

  // send a response that will be accepted, headers should be ok
  downstreamCodec_.generateHeader(output_, 1, resp, true, &size);
  EXPECT_GT(size.uncompressed, 0);

  // parse the remainder
  parseUpstream();
  callbacks_.expectMessage(true, 1, 200);
}