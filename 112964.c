ErrorCode HTTP2Codec::handleSettings(const std::deque<SettingPair>& settings) {
  SettingsList settingsList;
  for (auto& setting: settings) {
    switch (setting.first) {
      case SettingsId::HEADER_TABLE_SIZE:
      {
        uint32_t tableSize = setting.second;
        if (setting.second > http2::kMaxHeaderTableSize) {
          VLOG(2) << "Limiting table size from " << tableSize << " to " <<
            http2::kMaxHeaderTableSize;
          tableSize = http2::kMaxHeaderTableSize;
        }
        headerCodec_.setEncoderHeaderTableSize(tableSize);
      }
      break;
      case SettingsId::ENABLE_PUSH:
        if ((setting.second != 0 && setting.second != 1) ||
            (setting.second == 1 &&
             transportDirection_ == TransportDirection::UPSTREAM)) {
          goawayErrorMessage_ = folly::to<string>(
              "GOAWAY error: ENABLE_PUSH invalid setting=", setting.second,
              " for streamID=", curHeader_.stream);
          VLOG(4) << goawayErrorMessage_;
          return ErrorCode::PROTOCOL_ERROR;
        }
        break;
      case SettingsId::MAX_CONCURRENT_STREAMS:
        break;
      case SettingsId::INITIAL_WINDOW_SIZE:
        if (setting.second > http2::kMaxWindowUpdateSize) {
          goawayErrorMessage_ = folly::to<string>(
              "GOAWAY error: INITIAL_WINDOW_SIZE invalid size=", setting.second,
              " for streamID=", curHeader_.stream);
          VLOG(4) << goawayErrorMessage_;
          return ErrorCode::PROTOCOL_ERROR;
        }
        break;
      case SettingsId::MAX_FRAME_SIZE:
        if (setting.second < http2::kMaxFramePayloadLengthMin ||
            setting.second > http2::kMaxFramePayloadLength) {
          goawayErrorMessage_ = folly::to<string>(
              "GOAWAY error: MAX_FRAME_SIZE invalid size=", setting.second,
              " for streamID=", curHeader_.stream);
          VLOG(4) << goawayErrorMessage_;
          return ErrorCode::PROTOCOL_ERROR;
        }
        ingressSettings_.setSetting(SettingsId::MAX_FRAME_SIZE, setting.second);
        break;
      case SettingsId::MAX_HEADER_LIST_SIZE:
        break;
      case SettingsId::ENABLE_EX_HEADERS:
      {
        auto ptr = egressSettings_.getSetting(SettingsId::ENABLE_EX_HEADERS);
        if (ptr && ptr->value > 0) {
          VLOG(4) << getTransportDirectionString(getTransportDirection())
                  << " got ENABLE_EX_HEADERS=" << setting.second;
          if (setting.second != 0 && setting.second != 1) {
            goawayErrorMessage_ = folly::to<string>(
              "GOAWAY error: invalid ENABLE_EX_HEADERS=", setting.second,
              " for streamID=", curHeader_.stream);
            VLOG(4) << goawayErrorMessage_;
            return ErrorCode::PROTOCOL_ERROR;
          }
          break;
        } else {
          // egress ENABLE_EX_HEADERS is disabled, consider the ingress
          // ENABLE_EX_HEADERS as unknown setting, and ignore it.
          continue;
        }
      }
      case SettingsId::ENABLE_CONNECT_PROTOCOL:
        if (setting.second > 1) {
          goawayErrorMessage_ = folly::to<string>(
              "GOAWAY error: ENABLE_CONNECT_PROTOCOL invalid number=",
              setting.second, " for streamID=", curHeader_.stream);
          VLOG(4) << goawayErrorMessage_;
          return ErrorCode::PROTOCOL_ERROR;
        }
        break;
      case SettingsId::THRIFT_CHANNEL_ID:
      case SettingsId::THRIFT_CHANNEL_ID_DEPRECATED:
        break;
      case SettingsId::SETTINGS_HTTP_CERT_AUTH:
        break;
      default:
        continue; // ignore unknown setting
    }
    ingressSettings_.setSetting(setting.first, setting.second);
    settingsList.push_back(*ingressSettings_.getSetting(setting.first));
  }
  if (callback_) {
    callback_->onSettings(settingsList);
  }
  return ErrorCode::NO_ERROR;
}