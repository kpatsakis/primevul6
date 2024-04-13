size_t HTTP2Codec::generateRstStream(folly::IOBufQueue& writeBuf,
                                     StreamID stream,
                                     ErrorCode statusCode) {
  VLOG(4) << "sending RST_STREAM for stream=" << stream
          << " with code=" << getErrorCodeString(statusCode);
  if (!isStreamIngressEgressAllowed(stream)) {
    VLOG(2) << "suppressed RST_STREAM for stream=" << stream
            << " ingressGoawayAck_=" << ingressGoawayAck_;
    return 0;
  }
  // Suppress any EOM callback for the current frame.
  if (stream == curHeader_.stream) {
    curHeader_.flags &= ~http2::END_STREAM;
    pendingEndStreamHandling_ = false;
    ingressWebsocketUpgrade_ = false;
  }
  upgradedStreams_.erase(stream);

  if (statusCode == ErrorCode::PROTOCOL_ERROR) {
    VLOG(2) << "sending RST_STREAM with code=" << getErrorCodeString(statusCode)
            << " for stream=" << stream << " user-agent=" << userAgent_;
  }
  auto code = http2::errorCodeToReset(statusCode);
  return generateHeaderCallbackWrapper(stream, http2::FrameType::RST_STREAM,
                                       http2::writeRstStream(writeBuf, stream, code));
}