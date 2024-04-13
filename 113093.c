size_t HTTP2Codec::generatePriority(folly::IOBufQueue& writeBuf,
                                    StreamID stream,
                                    const HTTPMessage::HTTPPriority& pri) {
  VLOG(4) << "generating priority for stream=" << stream;
  if (!isStreamIngressEgressAllowed(stream)) {
    VLOG(2) << "suppressed PRIORITY for stream=" << stream
            << " ingressGoawayAck_=" << ingressGoawayAck_;
    return 0;
  }
  return generateHeaderCallbackWrapper(
            stream,
            http2::FrameType::PRIORITY,
            http2::writePriority(writeBuf, stream,
                                 {std::get<0>(pri),
                                   std::get<1>(pri),
                                   std::get<2>(pri)}));
}