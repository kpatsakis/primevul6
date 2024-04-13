void HTTP2Codec::generateHeader(folly::IOBufQueue& writeBuf,
                                StreamID stream,
                                const HTTPMessage& msg,
                                bool eom,
                                HTTPHeaderSize* size) {
  generateHeaderImpl(writeBuf,
                     stream,
                     msg,
                     folly::none, /* assocStream */
                     folly::none, /* controlStream */
                     eom,
                     size);
}