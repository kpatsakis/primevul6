ErrorCode HTTP2Codec::parseFrame(folly::io::Cursor& cursor) {
  FOLLY_SCOPED_TRACE_SECTION("HTTP2Codec - parseFrame");
  if (expectedContinuationStream_ != 0 &&
       (curHeader_.type != http2::FrameType::CONTINUATION ||
        expectedContinuationStream_ != curHeader_.stream)) {
    goawayErrorMessage_ = folly::to<string>(
        "GOAWAY error: while expected CONTINUATION with stream=",
        expectedContinuationStream_, ", received streamID=", curHeader_.stream,
        " of type=", getFrameTypeString(curHeader_.type));
    VLOG(4) << goawayErrorMessage_;
    return ErrorCode::PROTOCOL_ERROR;
  }
  if (expectedContinuationStream_ == 0 &&
      curHeader_.type == http2::FrameType::CONTINUATION) {
    goawayErrorMessage_ = folly::to<string>(
        "GOAWAY error: unexpected CONTINUATION received with streamID=",
        curHeader_.stream);
    VLOG(4) << goawayErrorMessage_;
    return ErrorCode::PROTOCOL_ERROR;
  }
  if (frameAffectsCompression(curHeader_.type) &&
      curHeaderBlock_.chainLength() + curHeader_.length >
      egressSettings_.getSetting(SettingsId::MAX_HEADER_LIST_SIZE, 0)) {
    // this may be off by up to the padding length (max 255), but
    // these numbers are already so generous, and we're comparing the
    // max-uncompressed to the actual compressed size.  Let's fail
    // before buffering.

    // TODO(t6513634): it would be nicer to stream-process this header
    // block to keep the connection state consistent without consuming
    // memory, and fail just the request per the HTTP/2 spec (section
    // 10.3)
    goawayErrorMessage_ = folly::to<string>(
      "Failing connection due to excessively large headers");
    LOG(ERROR) << goawayErrorMessage_;
    return ErrorCode::PROTOCOL_ERROR;
  }

  expectedContinuationStream_ =
    (frameAffectsCompression(curHeader_.type) &&
     !(curHeader_.flags & http2::END_HEADERS)) ? curHeader_.stream : 0;

  switch (curHeader_.type) {
    case http2::FrameType::DATA:
      return parseAllData(cursor);
    case http2::FrameType::HEADERS:
      return parseHeaders(cursor);
    case http2::FrameType::PRIORITY:
      return parsePriority(cursor);
    case http2::FrameType::RST_STREAM:
      return parseRstStream(cursor);
    case http2::FrameType::SETTINGS:
      return parseSettings(cursor);
    case http2::FrameType::PUSH_PROMISE:
      return parsePushPromise(cursor);
    case http2::FrameType::EX_HEADERS:
      if (ingressSettings_.getSetting(SettingsId::ENABLE_EX_HEADERS, 0)) {
        return parseExHeaders(cursor);
      } else {
        VLOG(2) << "EX_HEADERS not enabled, ignoring the frame";
        break;
      }
    case http2::FrameType::PING:
      return parsePing(cursor);
    case http2::FrameType::GOAWAY:
      return parseGoaway(cursor);
    case http2::FrameType::WINDOW_UPDATE:
      return parseWindowUpdate(cursor);
    case http2::FrameType::CONTINUATION:
      return parseContinuation(cursor);
    case http2::FrameType::ALTSVC:
      // fall through, unimplemented
      break;
    case http2::FrameType::CERTIFICATE_REQUEST:
      return parseCertificateRequest(cursor);
    case http2::FrameType::CERTIFICATE:
      return parseCertificate(cursor);
    default:
      // Implementations MUST ignore and discard any frame that has a
      // type that is unknown
      break;
  }

  // Landing here means unknown, unimplemented or ignored frame.
  VLOG(2) << "Skipping frame (type=" << (uint8_t)curHeader_.type << ")";
  cursor.skip(curHeader_.length);
  return ErrorCode::NO_ERROR;
}