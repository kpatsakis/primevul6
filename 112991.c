size_t HTTP2Codec::generatePingRequest(folly::IOBufQueue& writeBuf) {
  // should probably let the caller specify when integrating with session
  // we know HTTPSession sets up events to track ping latency
  uint64_t opaqueData = folly::Random::rand64();
  VLOG(4) << "Generating ping request with opaqueData=" << opaqueData;
  return generateHeaderCallbackWrapper(0, http2::FrameType::PING,
                                       http2::writePing(writeBuf, opaqueData, false /* no ack */));
}