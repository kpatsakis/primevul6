ErrorCode HTTP2Codec::parseHeaders(Cursor& cursor) {
  FOLLY_SCOPED_TRACE_SECTION("HTTP2Codec - parseHeaders");
  folly::Optional<http2::PriorityUpdate> priority;
  std::unique_ptr<IOBuf> headerBuf;
  VLOG(4) << "parsing HEADERS frame for stream=" << curHeader_.stream <<
    " length=" << curHeader_.length;
  auto err = http2::parseHeaders(cursor, curHeader_, priority, headerBuf);
  RETURN_IF_ERROR(err);
  if (transportDirection_ == TransportDirection::DOWNSTREAM) {
    RETURN_IF_ERROR(
        checkNewStream(curHeader_.stream, true /* trailersAllowed */));
  }
  err = parseHeadersImpl(cursor, std::move(headerBuf), priority, folly::none,
                         folly::none);
  return err;
}