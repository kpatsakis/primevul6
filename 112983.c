ErrorCode HTTP2Codec::parsePushPromise(Cursor& cursor) {
  // stream id must be idle - protocol error
  // assoc-stream-id=closed/unknown - protocol error, unless rst_stream sent

  /*
   * What does "must handle" mean in the following context?  I have to
   * accept this as a valid pushed resource?

    However, an endpoint that has sent RST_STREAM on the associated
    stream MUST handle PUSH_PROMISE frames that might have been
    created before the RST_STREAM frame is received and processed.
  */
  if (transportDirection_ != TransportDirection::UPSTREAM) {
    goawayErrorMessage_ = folly::to<string>(
      "Received PUSH_PROMISE on DOWNSTREAM codec");
    VLOG(2) << goawayErrorMessage_;
    return ErrorCode::PROTOCOL_ERROR;
  }
  if (egressSettings_.getSetting(SettingsId::ENABLE_PUSH, -1) != 1) {
    goawayErrorMessage_ = folly::to<string>(
      "Received PUSH_PROMISE on codec with push disabled");
    VLOG(2) << goawayErrorMessage_;
    return ErrorCode::PROTOCOL_ERROR;
  }
  VLOG(4) << "parsing PUSH_PROMISE frame for stream=" << curHeader_.stream <<
    " length=" << curHeader_.length;
  uint32_t promisedStream;
  std::unique_ptr<IOBuf> headerBlockFragment;
  auto err = http2::parsePushPromise(cursor, curHeader_, promisedStream,
                                     headerBlockFragment);
  RETURN_IF_ERROR(err);
  RETURN_IF_ERROR(checkNewStream(promisedStream, false /* trailersAllowed */));
  err = parseHeadersImpl(cursor, std::move(headerBlockFragment), folly::none,
                         promisedStream, folly::none);
  return err;
}