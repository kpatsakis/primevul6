size_t HTTP2Codec::generateWindowUpdate(folly::IOBufQueue& writeBuf,
                                        StreamID stream,
                                        uint32_t delta) {
  VLOG(4) << "generating window update for stream=" << stream
          << ": Processed " << delta << " bytes";
  if (!isStreamIngressEgressAllowed(stream)) {
    VLOG(2) << "suppressed WINDOW_UPDATE for stream=" << stream
            << " ingressGoawayAck_=" << ingressGoawayAck_;
    return 0;
  }
  return generateHeaderCallbackWrapper(stream, http2::FrameType::WINDOW_UPDATE,
                                       http2::writeWindowUpdate(writeBuf, stream, delta));
}