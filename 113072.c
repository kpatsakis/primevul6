size_t HTTP2Codec::generateGoaway(folly::IOBufQueue& writeBuf,
                                  StreamID lastStream,
                                  ErrorCode statusCode,
                                  std::unique_ptr<folly::IOBuf> debugData) {
  DCHECK_LE(lastStream, egressGoawayAck_) << "Cannot increase last good stream";
  egressGoawayAck_ = lastStream;
  if (sessionClosing_ == ClosingState::CLOSED) {
    VLOG(4) << "Not sending GOAWAY for closed session";
    return 0;
  }
  switch (sessionClosing_) {
    case ClosingState::OPEN:
    case ClosingState::OPEN_WITH_GRACEFUL_DRAIN_ENABLED:
      if (lastStream == std::numeric_limits<int32_t>::max() &&
          statusCode == ErrorCode::NO_ERROR) {
        sessionClosing_ = ClosingState::FIRST_GOAWAY_SENT;
      } else {
        // The user of this codec decided not to do the double goaway
        // drain, or this is not a graceful shutdown
        sessionClosing_ = ClosingState::CLOSED;
      }
      break;
    case ClosingState::FIRST_GOAWAY_SENT:
      sessionClosing_ = ClosingState::CLOSED;
      break;
    case ClosingState::CLOSING:
    case ClosingState::CLOSED:
      LOG(FATAL) << "unreachable";
  }

  VLOG(4) << "Sending GOAWAY with last acknowledged stream="
          << lastStream << " with code=" << getErrorCodeString(statusCode);
  if (statusCode == ErrorCode::PROTOCOL_ERROR) {
    VLOG(2) << "sending GOAWAY with last acknowledged stream=" << lastStream
            << " with code=" << getErrorCodeString(statusCode)
            << " user-agent=" << userAgent_;
  }

  auto code = http2::errorCodeToGoaway(statusCode);
  return generateHeaderCallbackWrapper(
            0,
            http2::FrameType::GOAWAY,
            http2::writeGoaway(writeBuf,
                              lastStream,
                              code,
                              std::move(debugData)));
}