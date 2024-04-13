size_t HTTP2Codec::generateChunkTerminator(folly::IOBufQueue& /*writeBuf*/,
                                           StreamID /*stream*/) {
  // HTTP/2 has no chunk terminators
  return 0;
}