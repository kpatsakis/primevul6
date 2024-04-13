size_t HTTP2Codec::generateSettingsAck(folly::IOBufQueue& writeBuf) {
  VLOG(4) << getTransportDirectionString(getTransportDirection())
          << " generating settings ack";
  return generateHeaderCallbackWrapper(0, http2::FrameType::SETTINGS,
                                       http2::writeSettingsAck(writeBuf));
}