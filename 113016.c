ErrorCode HTTP2Codec::parseSettings(Cursor& cursor) {
  VLOG(4) << "parsing SETTINGS frame for stream=" << curHeader_.stream <<
    " length=" << curHeader_.length;
  std::deque<SettingPair> settings;
  auto err = http2::parseSettings(cursor, curHeader_, settings);
  RETURN_IF_ERROR(err);
  if (curHeader_.flags & http2::ACK) {
    handleSettingsAck();
    return ErrorCode::NO_ERROR;
  }
  return handleSettings(settings);
}