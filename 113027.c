void HTTP2Codec::handleSettingsAck() {
  if (pendingTableMaxSize_) {
    headerCodec_.setDecoderHeaderTableMaxSize(*pendingTableMaxSize_);
    pendingTableMaxSize_ = folly::none;
  }
  if (callback_) {
    callback_->onSettingsAck();
  }
}