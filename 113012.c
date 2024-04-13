ErrorCode HTTP2Codec::parsePing(Cursor& cursor) {
  VLOG(4) << "parsing PING frame length=" << curHeader_.length;
  uint64_t opaqueData = 0;
  auto err = http2::parsePing(cursor, curHeader_, opaqueData);
  RETURN_IF_ERROR(err);
  if (callback_) {
    if (curHeader_.flags & http2::ACK) {
      callback_->onPingReply(opaqueData);
    } else {
      callback_->onPingRequest(opaqueData);
    }
  }
  return ErrorCode::NO_ERROR;
}