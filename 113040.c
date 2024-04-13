size_t HTTP2Codec::onIngress(const folly::IOBuf& buf) {
  // TODO: ensure only 1 parse at a time on stack.
  FOLLY_SCOPED_TRACE_SECTION("HTTP2Codec - onIngress");

  Cursor cursor(&buf);
  size_t parsed = 0;
  ErrorCode connError = ErrorCode::NO_ERROR;
  for (auto bufLen = cursor.totalLength();
       connError == ErrorCode::NO_ERROR;
       bufLen = cursor.totalLength()) {
    if (frameState_ == FrameState::UPSTREAM_CONNECTION_PREFACE) {
      if (bufLen >= http2::kConnectionPreface.length()) {
        auto test = cursor.readFixedString(http2::kConnectionPreface.length());
        parsed += http2::kConnectionPreface.length();
        if (test != http2::kConnectionPreface) {
          goawayErrorMessage_ = "missing connection preface";
          VLOG(4) << goawayErrorMessage_;
          connError = ErrorCode::PROTOCOL_ERROR;
        }
        frameState_ = FrameState::FRAME_HEADER;
      } else {
        break;
      }
    } else if (frameState_ == FrameState::FRAME_HEADER ||
               frameState_ == FrameState::DOWNSTREAM_CONNECTION_PREFACE) {
      // Waiting to parse the common frame header
      if (bufLen >= http2::kFrameHeaderSize) {
        connError = parseFrameHeader(cursor, curHeader_);
        parsed += http2::kFrameHeaderSize;
        if (frameState_ == FrameState::DOWNSTREAM_CONNECTION_PREFACE &&
            curHeader_.type != http2::FrameType::SETTINGS) {
          goawayErrorMessage_ = folly::to<string>(
              "GOAWAY error: got invalid connection preface frame type=",
              getFrameTypeString(curHeader_.type), "(", curHeader_.type, ")",
              " for streamID=", curHeader_.stream);
          VLOG(4) << goawayErrorMessage_;
          connError = ErrorCode::PROTOCOL_ERROR;
        }
        if (curHeader_.length > maxRecvFrameSize()) {
          VLOG(4) << "Excessively large frame len=" << curHeader_.length;
          connError = ErrorCode::FRAME_SIZE_ERROR;
        }

        if (callback_) {
          callback_->onFrameHeader(
            curHeader_.stream,
            curHeader_.flags,
            curHeader_.length,
            static_cast<uint8_t>(curHeader_.type));
        }

        frameState_ = (curHeader_.type == http2::FrameType::DATA) ?
          FrameState::DATA_FRAME_DATA : FrameState::FRAME_DATA;
        pendingDataFrameBytes_ = curHeader_.length;
        pendingDataFramePaddingBytes_ = 0;
#ifndef NDEBUG
        receivedFrameCount_++;
#endif
      } else {
        break;
      }
    } else if (frameState_ == FrameState::DATA_FRAME_DATA && bufLen > 0 &&
               (bufLen < curHeader_.length ||
                pendingDataFrameBytes_ < curHeader_.length)) {
      // FrameState::DATA_FRAME_DATA with partial data only
      size_t dataParsed = 0;
      connError = parseDataFrameData(cursor, bufLen, dataParsed);
      if (dataParsed == 0 && pendingDataFrameBytes_ > 0) {
        // We received only the padding byte, we will wait for more
        break;
      } else {
        parsed += dataParsed;
        if (pendingDataFrameBytes_ == 0) {
          frameState_ = FrameState::FRAME_HEADER;
        }
      }
    } else { // FrameState::FRAME_DATA
             // or FrameState::DATA_FRAME_DATA with all data available
      // Already parsed the common frame header
      const auto frameLen = curHeader_.length;
      if (bufLen >= frameLen) {
        connError = parseFrame(cursor);
        parsed += curHeader_.length;
        frameState_ = FrameState::FRAME_HEADER;
      } else {
        break;
      }
    }
  }
  checkConnectionError(connError, &buf);
  return parsed;
}