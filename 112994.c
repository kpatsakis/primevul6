size_t HTTP2Codec::generateBody(folly::IOBufQueue& writeBuf,
                                StreamID stream,
                                std::unique_ptr<folly::IOBuf> chain,
                                folly::Optional<uint8_t> padding,
                                bool eom) {
  // todo: generate random padding for everything?
  size_t written = 0;
  if (!isStreamIngressEgressAllowed(stream)) {
    VLOG(2) << "Suppressing DATA for stream=" << stream << " ingressGoawayAck_="
            << ingressGoawayAck_;
    return 0;
  }
  IOBufQueue queue(IOBufQueue::cacheChainLength());
  queue.append(std::move(chain));
  size_t maxFrameSize = maxSendFrameSize();
  while (queue.chainLength() > maxFrameSize) {
    auto chunk = queue.split(maxFrameSize);
    written += generateHeaderCallbackWrapper(
                  stream,
                  http2::FrameType::DATA,
                  http2::writeData(writeBuf,
                                   std::move(chunk),
                                   stream,
                                   padding,
                                   false,
                                   reuseIOBufHeadroomForData_));
  }

  return written + generateHeaderCallbackWrapper(
                      stream,
                      http2::FrameType::DATA,
                      http2::writeData(writeBuf,
                                       queue.move(),
                                       stream,
                                       padding,
                                       eom,
                                       reuseIOBufHeadroomForData_));
}