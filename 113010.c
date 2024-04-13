size_t HTTP2Codec::generateCertificate(folly::IOBufQueue& writeBuf,
                                       uint16_t certId,
                                       std::unique_ptr<folly::IOBuf> certData) {
  size_t written = 0;
  VLOG(4) << "sending CERTIFICATE with Cert-ID=" << certId << "for stream=0";
  IOBufQueue queue(IOBufQueue::cacheChainLength());
  queue.append(std::move(certData));
  // The maximum size of an autenticator fragment, combined with the Cert-ID can
  // not exceed the maximal allowable size of a sent frame.
  size_t maxChunkSize = maxSendFrameSize() - sizeof(certId);
  while (queue.chainLength() > maxChunkSize) {
    auto chunk = queue.splitAtMost(maxChunkSize);
    written +=
        http2::writeCertificate(writeBuf, certId, std::move(chunk), true);
  }
  return written +
         http2::writeCertificate(writeBuf, certId, queue.move(), false);
}