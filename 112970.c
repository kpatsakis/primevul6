ErrorCode HTTP2Codec::parseAllData(Cursor& cursor) {
  std::unique_ptr<IOBuf> outData;
  uint16_t padding = 0;
  VLOG(10) << "parsing all frame DATA bytes for stream=" << curHeader_.stream <<
    " length=" << curHeader_.length;
  auto ret = http2::parseData(cursor, curHeader_, outData, padding);
  RETURN_IF_ERROR(ret);

  if (callback_ && (padding > 0 || (outData && !outData->empty()))) {
    if (!outData) {
      outData = std::make_unique<IOBuf>();
    }
    deliverCallbackIfAllowed(&HTTPCodec::Callback::onBody, "onBody",
                             curHeader_.stream, std::move(outData), padding);
  }
  return handleEndStream();
}