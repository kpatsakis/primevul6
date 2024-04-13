size_t HTTP2Codec::generateCertificateRequest(
    folly::IOBufQueue& writeBuf,
    uint16_t requestId,
    std::unique_ptr<folly::IOBuf> certificateRequestData) {
  VLOG(4) << "generating CERTIFICATE_REQUEST with Request-ID=" << requestId;
  return http2::writeCertificateRequest(
      writeBuf, requestId, std::move(certificateRequestData));
}