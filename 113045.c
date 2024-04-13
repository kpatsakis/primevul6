bool HTTP2Codec::parsingTrailers() const {
  // HEADERS frame is used for request/response headers and trailers.
  // Per spec, specific role of HEADERS frame is determined by it's postion
  // within the stream. We don't keep full stream state in this codec,
  // thus using heuristics to distinguish between headers/trailers.
  // For DOWNSTREAM case, request headers HEADERS frame would be creating
  // new stream, thus HEADERS on existing stream ID are considered trailers
  // (see checkNewStream).
  // For UPSTREAM case, response headers are required to have status code,
  // thus if no status code we consider that trailers.
  if (curHeader_.type == http2::FrameType::HEADERS ||
      curHeader_.type == http2::FrameType::CONTINUATION) {
    if (transportDirection_ == TransportDirection::DOWNSTREAM) {
      return parsingDownstreamTrailers_;
    } else {
      return !decodeInfo_.hasStatus();
    }
  }
  return false;
}