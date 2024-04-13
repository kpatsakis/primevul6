size_t HTTP2Codec::generateEOM(folly::IOBufQueue& writeBuf,
                               StreamID stream) {
  VLOG(4) << "sending EOM for stream=" << stream;
  upgradedStreams_.erase(stream);
  if (!isStreamIngressEgressAllowed(stream)) {
    VLOG(2) << "suppressed EOM for stream=" << stream << " ingressGoawayAck_="
            << ingressGoawayAck_;
    return 0;
  }
  return generateHeaderCallbackWrapper(
            stream,
            http2::FrameType::DATA,
            http2::writeData(writeBuf,
                             nullptr,
                             stream,
                             http2::kNoPadding,
                             true,
                             reuseIOBufHeadroomForData_));
}