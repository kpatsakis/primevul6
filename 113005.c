ErrorCode HTTP2Codec::parseRstStream(Cursor& cursor) {
  // rst for stream in idle state - protocol error
  VLOG(4) << "parsing RST_STREAM frame for stream=" << curHeader_.stream <<
    " length=" << curHeader_.length;
  upgradedStreams_.erase(curHeader_.stream);
  ErrorCode statusCode = ErrorCode::NO_ERROR;
  auto err = http2::parseRstStream(cursor, curHeader_, statusCode);
  RETURN_IF_ERROR(err);
  if (statusCode == ErrorCode::PROTOCOL_ERROR) {
    goawayErrorMessage_ = folly::to<string>(
        "GOAWAY error: RST_STREAM with code=", getErrorCodeString(statusCode),
        " for streamID=", curHeader_.stream, " user-agent=", userAgent_);
    VLOG(2) << goawayErrorMessage_;
  }
  deliverCallbackIfAllowed(&HTTPCodec::Callback::onAbort, "onAbort",
                           curHeader_.stream, statusCode);
  return ErrorCode::NO_ERROR;
}