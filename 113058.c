ErrorCode HTTP2Codec::parseDataFrameData(Cursor& cursor,
                                         size_t bufLen,
                                         size_t& parsed) {
  FOLLY_SCOPED_TRACE_SECTION("HTTP2Codec - parseDataFrameData");
  if (bufLen == 0) {
    VLOG(10) << "No data to parse";
    return ErrorCode::NO_ERROR;
  }

  std::unique_ptr<IOBuf> outData;
  uint16_t padding = 0;
  VLOG(10) << "parsing DATA frame data for stream=" << curHeader_.stream <<
    " frame data length=" << curHeader_.length << " pendingDataFrameBytes_=" <<
    pendingDataFrameBytes_ << " pendingDataFramePaddingBytes_=" <<
    pendingDataFramePaddingBytes_ << " bufLen=" << bufLen <<
    " parsed=" << parsed;
  // Parse the padding information only the first time
  if (pendingDataFrameBytes_ == curHeader_.length &&
    pendingDataFramePaddingBytes_ == 0) {
    if (frameHasPadding(curHeader_) && bufLen == 1) {
      // We need to wait for more bytes otherwise we won't be able to pass
      // the correct padding to the first onBody call
      return ErrorCode::NO_ERROR;
    }
    const auto ret = http2::parseDataBegin(cursor, curHeader_, parsed, padding);
    RETURN_IF_ERROR(ret);
    if (padding > 0) {
      pendingDataFramePaddingBytes_ = padding - 1;
      pendingDataFrameBytes_--;
      bufLen--;
      parsed++;
    }
    VLOG(10) << "out padding=" << padding << " pendingDataFrameBytes_=" <<
      pendingDataFrameBytes_ << " pendingDataFramePaddingBytes_=" <<
      pendingDataFramePaddingBytes_ << " bufLen=" << bufLen <<
      " parsed=" << parsed;
  }
  if (bufLen > 0) {
    // Check if we have application data to parse
    if (pendingDataFrameBytes_ > pendingDataFramePaddingBytes_) {
      const size_t pendingAppData =
        pendingDataFrameBytes_ - pendingDataFramePaddingBytes_;
      const size_t toClone = std::min(pendingAppData, bufLen);
      cursor.clone(outData, toClone);
      bufLen -= toClone;
      pendingDataFrameBytes_ -= toClone;
      parsed += toClone;
      VLOG(10) << "parsed some app data, pendingDataFrameBytes_=" <<
        pendingDataFrameBytes_ << " pendingDataFramePaddingBytes_=" <<
        pendingDataFramePaddingBytes_ << " bufLen=" << bufLen <<
        " parsed=" << parsed;
    }
    // Check if we have padding bytes to parse
    if (bufLen > 0 && pendingDataFramePaddingBytes_ > 0) {
      size_t toSkip = 0;
      auto ret = http2::parseDataEnd(cursor, bufLen,
                                     pendingDataFramePaddingBytes_, toSkip);
      RETURN_IF_ERROR(ret);
      pendingDataFrameBytes_ -= toSkip;
      pendingDataFramePaddingBytes_ -= toSkip;
      parsed += toSkip;
      VLOG(10) << "parsed some padding, pendingDataFrameBytes_=" <<
        pendingDataFrameBytes_ << " pendingDataFramePaddingBytes_=" <<
        pendingDataFramePaddingBytes_ << " bufLen=" << bufLen <<
        " parsed=" << parsed;
    }
  }

  if (callback_ && (padding > 0 || (outData && !outData->empty()))) {
    if (!outData) {
      outData = std::make_unique<IOBuf>();
    }
    deliverCallbackIfAllowed(&HTTPCodec::Callback::onBody, "onBody",
                             curHeader_.stream, std::move(outData), padding);
  }
  return (pendingDataFrameBytes_ > 0) ? ErrorCode::NO_ERROR : handleEndStream();
}