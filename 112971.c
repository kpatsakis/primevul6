ErrorCode HTTP2Codec::parseContinuation(Cursor& cursor) {
  std::unique_ptr<IOBuf> headerBuf;
  VLOG(4) << "parsing CONTINUATION frame for stream=" << curHeader_.stream <<
    " length=" << curHeader_.length;
  auto err = http2::parseContinuation(cursor, curHeader_, headerBuf);
  RETURN_IF_ERROR(err);
  err = parseHeadersImpl(cursor, std::move(headerBuf),
                         folly::none, folly::none, folly::none);
  return err;
}