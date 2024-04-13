bool HTTP2Codec::onIngressUpgradeMessage(const HTTPMessage& msg) {
  if (!HTTPParallelCodec::onIngressUpgradeMessage(msg)) {
    return false;
  }
  if (msg.getHeaders().getNumberOfValues(http2::kProtocolSettingsHeader) != 1) {
    VLOG(4) << __func__ << " with no HTTP2-Settings";
    return false;
  }

  const auto& settingsHeader = msg.getHeaders().getSingleOrEmpty(
    http2::kProtocolSettingsHeader);
  if (settingsHeader.empty()) {
    return true;
  }

  auto decoded = base64url_decode(settingsHeader);

  // Must be well formed Base64Url and not too large
  if (decoded.empty() || decoded.length() > http2::kMaxFramePayloadLength) {
    VLOG(4) << __func__ << " failed to decode HTTP2-Settings";
    return false;
  }
  std::unique_ptr<IOBuf> decodedBuf = IOBuf::wrapBuffer(decoded.data(),
                                                        decoded.length());
  IOBufQueue settingsQueue{IOBufQueue::cacheChainLength()};
  settingsQueue.append(std::move(decodedBuf));
  Cursor c(settingsQueue.front());
  std::deque<SettingPair> settings;
  // downcast is ok because of above length check
  http2::FrameHeader frameHeader{
    (uint32_t)settingsQueue.chainLength(), 0, http2::FrameType::SETTINGS, 0, 0};
  auto err = http2::parseSettings(c, frameHeader, settings);
  if (err != ErrorCode::NO_ERROR) {
    VLOG(4) << __func__ << " bad settings frame";
    return false;
  }

  if (handleSettings(settings) != ErrorCode::NO_ERROR) {
    VLOG(4) << __func__ << " handleSettings failed";
    return false;
  }

  return true;
}