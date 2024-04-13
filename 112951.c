void HTTP2Codec::generatePushPromise(folly::IOBufQueue& writeBuf,
                                     StreamID stream,
                                     const HTTPMessage& msg,
                                     StreamID assocStream,
                                     bool eom,
                                     HTTPHeaderSize* size) {
  generateHeaderImpl(writeBuf,
                     stream,
                     msg,
                     assocStream,
                     folly::none, /* controlStream */
                     eom,
                     size);
}