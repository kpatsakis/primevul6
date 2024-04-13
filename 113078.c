HTTP2Codec::HTTP2Codec(TransportDirection direction)
    : HTTPParallelCodec(direction),
      headerCodec_(direction),
      frameState_(direction == TransportDirection::DOWNSTREAM
                  ? FrameState::UPSTREAM_CONNECTION_PREFACE
                  : FrameState::DOWNSTREAM_CONNECTION_PREFACE) {

  const auto maxHeaderListSize = egressSettings_.getSetting(
    SettingsId::MAX_HEADER_LIST_SIZE);
  if (maxHeaderListSize) {
    headerCodec_.setMaxUncompressed(maxHeaderListSize->value);
  }

  VLOG(4) << "creating " << getTransportDirectionString(direction)
          << " HTTP/2 codec";
}