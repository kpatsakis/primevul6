void HTTP2Codec::generateContinuation(folly::IOBufQueue& writeBuf,
                                      folly::IOBufQueue& queue,
                                      StreamID stream,
                                      size_t maxFrameSize) {
  bool endHeaders = false;
  while (!endHeaders) {
    auto chunk = queue.split(std::min(maxFrameSize, queue.chainLength()));
    endHeaders = (queue.chainLength() == 0);
    VLOG(4) << "generating CONTINUATION for stream=" << stream;
    generateHeaderCallbackWrapper(
        stream,
        http2::FrameType::CONTINUATION,
        http2::writeContinuation(
            writeBuf, stream, endHeaders, std::move(chunk)));
  }
}