void HTTP2Codec::requestUpgrade(HTTPMessage& request) {
  static folly::ThreadLocalPtr<HTTP2Codec> defaultCodec;
  if (!defaultCodec.get()) {
    defaultCodec.reset(new HTTP2Codec(TransportDirection::UPSTREAM));
  }

  auto& headers = request.getHeaders();
  headers.set(HTTP_HEADER_UPGRADE, http2::kProtocolCleartextString);
  if (!request.checkForHeaderToken(HTTP_HEADER_CONNECTION, "Upgrade", false)) {
    headers.add(HTTP_HEADER_CONNECTION, "Upgrade");
  }
  IOBufQueue writeBuf{IOBufQueue::cacheChainLength()};
  defaultCodec->generateSettings(writeBuf);
  // fake an ack since defaultCodec gets reused
  defaultCodec->handleSettingsAck();
  writeBuf.trimStart(http2::kFrameHeaderSize);
  auto buf = writeBuf.move();
  buf->coalesce();
  headers.set(http2::kProtocolSettingsHeader,
              base64url_encode(folly::ByteRange(buf->data(), buf->length())));
  if (!request.checkForHeaderToken(HTTP_HEADER_CONNECTION,
                                   http2::kProtocolSettingsHeader.c_str(),
                                   false)) {
    headers.add(HTTP_HEADER_CONNECTION, http2::kProtocolSettingsHeader);
  }
}