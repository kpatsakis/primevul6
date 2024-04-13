void HTTP2Codec::generateExHeader(folly::IOBufQueue& writeBuf,
                                  StreamID stream,
                                  const HTTPMessage& msg,
                                  const HTTPCodec::ExAttributes& exAttributes,
                                  bool eom,
                                  HTTPHeaderSize* size) {
  generateHeaderImpl(writeBuf,
                     stream,
                     msg,
                     folly::none, /* assocStream */
                     exAttributes,
                     eom,
                     size);
}