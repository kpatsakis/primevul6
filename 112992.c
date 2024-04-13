size_t HTTP2Codec::generateSettings(folly::IOBufQueue& writeBuf) {
  std::deque<SettingPair> settings;
  for (auto& setting: egressSettings_.getAllSettings()) {
    switch (setting.id) {
      case SettingsId::HEADER_TABLE_SIZE:
        if (pendingTableMaxSize_) {
          LOG(ERROR) << "Can't have more than one settings in flight, skipping";
          continue;
        } else {
          pendingTableMaxSize_ = setting.value;
        }
        break;
      case SettingsId::ENABLE_PUSH:
        if (transportDirection_ == TransportDirection::DOWNSTREAM) {
          // HTTP/2 spec says downstream must not send this flag
          // HTTP2Codec uses it to determine if push features are enabled
          continue;
        } else {
          CHECK(setting.value == 0 || setting.value == 1);
        }
        break;
      case SettingsId::MAX_CONCURRENT_STREAMS:
      case SettingsId::INITIAL_WINDOW_SIZE:
      case SettingsId::MAX_FRAME_SIZE:
        break;
      case SettingsId::MAX_HEADER_LIST_SIZE:
        headerCodec_.setMaxUncompressed(setting.value);
        break;
      case SettingsId::ENABLE_EX_HEADERS:
        CHECK(setting.value == 0 || setting.value == 1);
        if (setting.value == 0) {
          continue; // just skip the experimental setting if disabled
        } else {
          VLOG(4) << "generating ENABLE_EX_HEADERS=" << setting.value;
        }
        break;
      case SettingsId::ENABLE_CONNECT_PROTOCOL:
        if (setting.value == 0) {
          continue;
        }
        break;
      case SettingsId::THRIFT_CHANNEL_ID:
      case SettingsId::THRIFT_CHANNEL_ID_DEPRECATED:
        break;
      default:
        LOG(ERROR) << "ignore unknown settingsId="
                   << std::underlying_type<SettingsId>::type(setting.id)
                   << " value=" << setting.value;
        continue;
    }

    settings.push_back(SettingPair(setting.id, setting.value));
  }
  VLOG(4) << getTransportDirectionString(getTransportDirection())
          << " generating " << (unsigned)settings.size() << " settings";
  return generateHeaderCallbackWrapper(0, http2::FrameType::SETTINGS,
                                       http2::writeSettings(writeBuf, settings));
}