ErrorCode HTTP2Codec::parseExHeaders(Cursor& cursor) {
  FOLLY_SCOPED_TRACE_SECTION("HTTP2Codec - parseExHeaders");
  HTTPCodec::ExAttributes exAttributes;
  folly::Optional<http2::PriorityUpdate> priority;
  std::unique_ptr<IOBuf> headerBuf;
  VLOG(4) << "parsing ExHEADERS frame for stream=" << curHeader_.stream
          << " length=" << curHeader_.length;
  auto err = http2::parseExHeaders(
      cursor, curHeader_, exAttributes, priority, headerBuf);
  RETURN_IF_ERROR(err);
  if (isRequest(curHeader_.stream)) {
    RETURN_IF_ERROR(
        checkNewStream(curHeader_.stream, false /* trailersAllowed */));
  }
  return parseHeadersImpl(cursor, std::move(headerBuf), priority, folly::none,
                          exAttributes);
}