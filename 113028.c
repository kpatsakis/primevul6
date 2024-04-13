size_t HTTP2Codec::generatePingReply(folly::IOBufQueue& writeBuf,
                                     uint64_t uniqueID) {
  VLOG(4) << "Generating ping reply with opaqueData=" << uniqueID;
  return generateHeaderCallbackWrapper(0, http2::FrameType::PING,
                                       http2::writePing(writeBuf, uniqueID, true /* ack */));
}