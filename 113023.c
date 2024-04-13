ErrorCode HTTP2Codec::parseCertificate(Cursor& cursor) {
  VLOG(4) << "parsing CERTIFICATE frame length=" << curHeader_.length;
  uint16_t certId = 0;
  std::unique_ptr<IOBuf> authData;
  auto err = http2::parseCertificate(cursor, curHeader_, certId, authData);
  RETURN_IF_ERROR(err);
  if (curAuthenticatorBlock_.empty()) {
    curCertId_ = certId;
  } else if (certId != curCertId_) {
    // Received CERTIFICATE frame with different Cert-ID.
    return ErrorCode::PROTOCOL_ERROR;
  }
  curAuthenticatorBlock_.append(std::move(authData));
  if (curAuthenticatorBlock_.chainLength() > http2::kMaxAuthenticatorBufSize) {
    // Received excessively long authenticator.
    return ErrorCode::PROTOCOL_ERROR;
  }
  if (!(curHeader_.flags & http2::TO_BE_CONTINUED)) {
    auto authenticator = curAuthenticatorBlock_.move();
    if (callback_) {
      callback_->onCertificate(certId, std::move(authenticator));
    } else {
      curAuthenticatorBlock_.clear();
    }
  }
  return ErrorCode::NO_ERROR;
}