void HTTP2Codec::streamError(const std::string& msg, ErrorCode code,
                             bool newTxn) {
  HTTPException error(HTTPException::Direction::INGRESS_AND_EGRESS,
                      msg);
  error.setCodecStatusCode(code);
  if (callback_) {
    callback_->onError(curHeader_.stream, error, newTxn);
  }
}