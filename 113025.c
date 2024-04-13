TEST_F(HTTP2CodecTest, TestAllEgressFrameTypeCallbacks) {
  class CallbackTypeTracker {
    std::set<uint8_t> types;
  public:
    void add(uint8_t, uint8_t type, uint64_t, uint16_t) {
      types.insert(type);
    }

    bool isAllFrameTypesReceived() {
      http2::FrameType expectedTypes[] = {
        http2::FrameType::DATA,
        http2::FrameType::HEADERS,
        http2::FrameType::PRIORITY,
        http2::FrameType::RST_STREAM,
        http2::FrameType::SETTINGS,
        http2::FrameType::PUSH_PROMISE,
        http2::FrameType::PING,
        http2::FrameType::GOAWAY,
        http2::FrameType::WINDOW_UPDATE,
        http2::FrameType::CONTINUATION,
        http2::FrameType::EX_HEADERS,
      };

      for(http2::FrameType type: expectedTypes) {
        EXPECT_TRUE(types.find(static_cast<uint8_t>(type)) != types.end())
          << "callback missing for type " << static_cast<uint8_t>(type);
      }
      return types.size() == (sizeof(expectedTypes)/sizeof(http2::FrameType));
    }
  };

  CallbackTypeTracker callbackTypeTracker;

  NiceMock<MockHTTPCodecCallback> mockCallback;
  upstreamCodec_.setCallback(&mockCallback);
  downstreamCodec_.setCallback(&mockCallback);
  EXPECT_CALL(mockCallback, onGenerateFrameHeader(_, _, _, _)).
    WillRepeatedly(Invoke(&callbackTypeTracker, &CallbackTypeTracker::add));

  // DATA frame
  string data("abcde");
  auto buf = folly::IOBuf::copyBuffer(data.data(), data.length());
  upstreamCodec_.generateBody(output_, 2, std::move(buf),
                              HTTPCodec::NoPadding, true);

  HTTPHeaderSize size;
  size.uncompressed = size.compressed = 0;
  HTTPMessage req = getGetRequest();
  upstreamCodec_.generateHeader(output_, 1, req, true, &size);

  upstreamCodec_.generatePriority(output_, 3,
                                  HTTPMessage::HTTPPriority(0, true, 1));
  upstreamCodec_.generateRstStream(output_, 2, ErrorCode::ENHANCE_YOUR_CALM);
  upstreamCodec_.generateSettings(output_);
  downstreamCodec_.generatePushPromise(output_, 2, req, 1);
  upstreamCodec_.generatePingRequest(output_);

  std::unique_ptr<folly::IOBuf> debugData =
      folly::IOBuf::copyBuffer("debugData");
  upstreamCodec_.generateGoaway(output_, 17, ErrorCode::ENHANCE_YOUR_CALM,
                                std::move(debugData));

  upstreamCodec_.generateWindowUpdate(output_, 0, 10);

  HTTPCodec::StreamID stream = folly::Random::rand32(10, 1024) * 2;
  HTTPCodec::StreamID controlStream = folly::Random::rand32(10, 1024) * 2 + 1;
  downstreamCodec_.generateExHeader(output_, stream, req,
    HTTPCodec::ExAttributes(controlStream, true));

  // Tests the continuation frame
  req = getBigGetRequest();
  upstreamCodec_.generateHeader(output_, 1, req, true /* eom */);

  EXPECT_TRUE(callbackTypeTracker.isAllFrameTypesReceived());
}