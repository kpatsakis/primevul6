ErrorCode HTTP2Codec::handleEndStream() {
  if (curHeader_.type != http2::FrameType::HEADERS &&
      curHeader_.type != http2::FrameType::EX_HEADERS &&
      curHeader_.type != http2::FrameType::CONTINUATION &&
      curHeader_.type != http2::FrameType::DATA) {
    return ErrorCode::NO_ERROR;
  }

  // do we need to handle case where this stream has already aborted via
  // another callback (onHeadersComplete/onBody)?
  pendingEndStreamHandling_ |= (curHeader_.flags & http2::END_STREAM);

  // with a websocket upgrade, we need to send message complete cb to
  // mirror h1x codec's behavior. when the stream closes, we need to
  // send another callback to clean up the stream's resources.
  if (ingressWebsocketUpgrade_) {
    ingressWebsocketUpgrade_ = false;
    deliverCallbackIfAllowed(&HTTPCodec::Callback::onMessageComplete,
                             "onMessageComplete", curHeader_.stream, true);
  }

  if (pendingEndStreamHandling_ && expectedContinuationStream_ == 0) {
    pendingEndStreamHandling_ = false;
    deliverCallbackIfAllowed(&HTTPCodec::Callback::onMessageComplete,
                             "onMessageComplete", curHeader_.stream, false);
  }
  return ErrorCode::NO_ERROR;
}