ErrorCode HTTP2Codec::parseCertificateRequest(Cursor& cursor) {
  VLOG(4) << "parsing CERTIFICATE_REQUEST frame length=" << curHeader_.length;
  uint16_t requestId = 0;
  std::unique_ptr<IOBuf> authRequest;

  auto err = http2::parseCertificateRequest(
      cursor, curHeader_, requestId, authRequest);
  RETURN_IF_ERROR(err);
  if (callback_) {
    callback_->onCertificateRequest(requestId, std::move(authRequest));
  }
  return ErrorCode::NO_ERROR;
}