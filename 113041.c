bool HTTP2Codec::checkConnectionError(ErrorCode err, const folly::IOBuf* buf) {
  if (err != ErrorCode::NO_ERROR) {
    LOG(ERROR) << "Connection error " << getErrorCodeString(err)
               << " with ingress=";
    VLOG(3) << IOBufPrinter::printHexFolly(buf, true);
    if (callback_) {
      std::string errorDescription = goawayErrorMessage_.empty() ?
        "Connection error" : goawayErrorMessage_;
      HTTPException ex(HTTPException::Direction::INGRESS_AND_EGRESS,
                       errorDescription);
      ex.setCodecStatusCode(err);
      callback_->onError(0, ex, false);
    }
    return true;
  }
  return false;
}