size_t HTTP2Codec::generateHeaderCallbackWrapper(StreamID stream,
                                                 http2::FrameType type,
                                                 size_t length) {
  if (callback_) {
    callback_->onGenerateFrameHeader(stream,
                                     static_cast<uint8_t>(type),
                                     length);
  }
  return length;
}