size_t HTTP2Codec::generateChunkHeader(folly::IOBufQueue& /*writeBuf*/,
                                       StreamID /*stream*/,
                                       size_t /*length*/) {
  // HTTP/2 has no chunk headers
  return 0;
}