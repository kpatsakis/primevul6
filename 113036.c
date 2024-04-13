ErrorCode HTTP2Codec::parseGoaway(Cursor& cursor) {
  VLOG(4) << "parsing GOAWAY frame length=" << curHeader_.length;
  uint32_t lastGoodStream = 0;
  ErrorCode statusCode = ErrorCode::NO_ERROR;
  std::unique_ptr<IOBuf> debugData;

  auto err = http2::parseGoaway(cursor, curHeader_, lastGoodStream, statusCode,
                                debugData);
  if (statusCode != ErrorCode::NO_ERROR) {
    VLOG(2) << "Goaway error statusCode=" << getErrorCodeString(statusCode)
            << " lastStream=" << lastGoodStream
            << " user-agent=" << userAgent_ <<  " debugData=" <<
      ((debugData) ? string((char*)debugData->data(), debugData->length()):
       empty_string);
  }
  RETURN_IF_ERROR(err);
  if (lastGoodStream < ingressGoawayAck_) {
    ingressGoawayAck_ = lastGoodStream;
    // Drain all streams <= lastGoodStream
    // and abort streams > lastGoodStream
    if (callback_) {
      callback_->onGoaway(lastGoodStream, statusCode, std::move(debugData));
    }
  } else {
    LOG(WARNING) << "Received multiple GOAWAY with increasing ack";
  }
  return ErrorCode::NO_ERROR;
}